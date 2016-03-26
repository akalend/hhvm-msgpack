/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2015 Alexandre Kalendarev                              |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+

	 part this code was derived from:
		  tarantool/msgpuck, author Roman Tsisyk
		  utf-8 detect, author Bjoern Hoehrmann

	 for HHVM version 3.13

  KindOfUninit          = 0x00,  //  00000000
  KindOfNull            = 0x01,  //  00000001
  KindOfBoolean         = 0x09,  //  00001001  9
  KindOfInt64           = 0x11,  //  00010001 17
  KindOfDouble          = 0x19,  //  00011001 25
  KindOfPersistentString = 0x1b, //  00011011 27
  KindOfPersistentArray = 0x1d,  //  00011101 29
  KindOfString          = 0x22,  //  00100010 34
  KindOfArray           = 0x34,  //  00110100 52
  KindOfObject          = 0x40,  //  01000000

*/

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context
#include "hphp/runtime/base/type-object.h"  	  // Object
#include "hphp/runtime/base/builtin-functions.h"
#include "hphp/runtime/base/variable-serializer.h"
#include "hphp/runtime/base/variable-unserializer.h"


#define MP_SOURCE 1
#include "msgpuck.h"
#include "msgpack.h"

#define EXT_TYPE_OBJ 5

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const uint8_t utf8d[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
  0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
  0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
  0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
  1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
  1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
  1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
uint32_t inline
decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
  uint32_t type = utf8d[byte];

  *codep = (*state != UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

  *state = utf8d[256 + *state*16 + type];
  return *state;
}
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
int checkUTF8(uint8_t* s, size_t* count) {
  uint32_t codepoint;
  uint32_t state = 0;

  for (*count = 0; *s; ++s)
    if (!decode(&state, &codepoint, *s))
      *count += 1;

  return state != UTF8_ACCEPT;
}


namespace HPHP {

static void packVariant(const Variant& el);
static int sizeof_pack(const Array& data);
static int sizeof_el(const Variant& el);

static void packVariantLen(const Variant& el, int * len );

/**
* return true if PHP Array as map
*/
static bool checkIsMap(const Array& data) {
	int i =0;
	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
		pos = data->iter_advance(pos)) {
		const Variant key = data->getKey(pos);
		if (key.isInteger() != true) {
			return true;
		}
		if ( key.toInt64() != i) {
			return true;
		}

		i++;
	}

	return false;
}

static void encodeMapElementLen(const Variant& key, const Variant& val, int* len) {
	packVariantLen(key, len);
	packVariantLen(val, len);
}

static void encodeArrayElementLen(const Variant& val, int* len) {
	packVariantLen(val, len); 
}


static void mapIterationLen(ArrayData* data, int* len, void  (mapIterationCb) (const Variant& , const Variant&, int*)) {
	
	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
		   pos = data->iter_advance(pos)) {
		   const Variant key = data->getKey(pos);
		   const Variant val = data->getValue(pos);
		   mapIterationCb(key, val, len);
	}
}


static void arrayIterationLen(ArrayData* data, int* len,void  (arrayIterationCb) (const Variant&, int*)) {

	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
		   pos = data->iter_advance(pos)) {
		   const Variant val = data->getValue(pos);
		   arrayIterationCb(val, len);
	}
}

static void packVariantLen(const Variant& el, int * len ) {
	
	switch(el.getType()) {
		case KindOfInt64 : { 
		
			int64_t int_val = el.toInt64();
			if (int_val >= 0) {
				*len += mp_sizeof_uint( el.toInt64() );
			} else {
				*len += mp_sizeof_int( el.toInt64() );
			}

			break; }
		
		case KindOfNull : { 
			*len += mp_sizeof_nil();
			break; }

		case KindOfBoolean : { 
			*len += mp_sizeof_bool( el.toBoolean() );
			break; 
		}

		case KindOfPersistentString:
		case KindOfString : {

			*len +=  mp_sizeof_str( el.toString().length() );
			break;
		}
		
		case KindOfPersistentArray:
		case KindOfArray : {

				bool isMap = checkIsMap(el.toArray());
				ArrayData* ad = el.toArray().get();
				if (isMap) {
					*len += mp_sizeof_map(el.toArray().length());
					mapIterationLen(ad, len,encodeMapElementLen);
				} else {
					*len += mp_sizeof_array(el.toArray().length());
					arrayIterationLen(ad, len,encodeArrayElementLen);
				}

				break; 
			}

		case KindOfDouble : {
			*len = mp_sizeof_double( el.toDouble());
			break;
		}

		case KindOfObject: {
			VariableSerializer vs(VariableSerializer::Type::Serialize);
			String str_json (vs.serialize(el, true));
			*len = mp_sizeof_str( str_json.length() );
			break;
		}		

		default : {
			raise_warning("error type of data element");	
		}
	}
}


static int sizeof_pack( const Array& data ) {
		
	int size = 0;

	for (int i = 0; i < data.length(); ++i)	{
		packVariantLen(data[i], &size);
	}

	return size;
}

static void mapIteration(ArrayData* data, void  (mapIterationCb) (const Variant& , const Variant&)) {

	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
		   pos = data->iter_advance(pos)) {
		   const Variant key = data->getKey(pos);
		   const Variant val = data->getValue(pos);
		   mapIterationCb(key, val);
	}
}


static void arrayIteration(ArrayData* data, void  (arrayIterationCb) (const Variant&)) {

	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
		   pos = data->iter_advance(pos)) {
		   const Variant val = data->getValue(pos);
		   arrayIterationCb(val);
	}
}

static void encodeMapElement(const Variant& key, const Variant& val) {
	packVariant(key);
	packVariant(val); 
}

static void encodeArrayElement(const Variant& val) {
	packVariant(val); 
}



static void packVariant(const Variant& el) {
	
	switch(el.getType()) {
		case KindOfInt64 : { 
			int64_t int_val = el.toInt64();
			if (int_val >= 0) {
				MsgpackExtension::BufferPtr = mp_encode_uint(MsgpackExtension::BufferPtr,  int_val );
			} else {
				MsgpackExtension::BufferPtr = mp_encode_int(MsgpackExtension::BufferPtr,  int_val );
			}
			break; }
		
		case KindOfNull : { 
			MsgpackExtension::BufferPtr = mp_encode_nil(MsgpackExtension::BufferPtr);
			break; }

		case KindOfBoolean : { 
			MsgpackExtension::BufferPtr = mp_encode_bool(MsgpackExtension::BufferPtr, el.toBoolean());
			break; 
		}

		case KindOfPersistentString:
		case KindOfString : {

			// check UTF-8
			uint8_t * p = (uint8_t*)el.toString().c_str();
			size_t count = static_cast<size_t>(el.toString().length());
			int isBinary = checkUTF8(p, &count);

			if (isBinary)
				MsgpackExtension::BufferPtr = mp_encode_bin(MsgpackExtension::BufferPtr, el.toString().c_str(), el.toString().length());			
			else
				MsgpackExtension::BufferPtr = mp_encode_str(MsgpackExtension::BufferPtr, el.toString().c_str(), el.toString().length());
			
			break;
		}
		
		case KindOfPersistentArray:
		case KindOfArray : {

				bool isMap = checkIsMap(el.toArray());
				ArrayData* ad = el.toArray().get();
				if (isMap) {
					MsgpackExtension::BufferPtr = mp_encode_map(MsgpackExtension::BufferPtr, el.toArray().length());	
					mapIteration(ad, encodeMapElement);
				} else {
					MsgpackExtension::BufferPtr = mp_encode_array(MsgpackExtension::BufferPtr, el.toArray().length());
					arrayIteration(ad, encodeArrayElement);
				}

				break; 
			}

		case KindOfDouble : {
			MsgpackExtension::BufferPtr = mp_encode_double(MsgpackExtension::BufferPtr, el.toDouble());
			break;
		}
		
		case KindOfObject: {

			bool isMap = el.toObject().instanceof("\\HH\\Map");
			if (isMap) {
				ArrayData* ad = el.toArray().get();
				printf("serialize Map\n");
				MsgpackExtension::BufferPtr = mp_encode_map(MsgpackExtension::BufferPtr, el.toArray().length());	
				mapIteration(ad, encodeMapElement);
				break;					
			}

			bool isVector = el.toObject().instanceof("\\HH\\Vector");
			if (isVector) {
				printf("serialize Vector\n");
				ArrayData* ad = el.toArray().get();
				MsgpackExtension::BufferPtr = mp_encode_array(MsgpackExtension::BufferPtr, el.toArray().length());
				arrayIteration(ad, encodeArrayElement);
			}

			VariableSerializer vs(VariableSerializer::Type::Serialize);
			String str_json (vs.serialize(el, true));
			MsgpackExtension::BufferPtr = mp_encode_ext(MsgpackExtension::BufferPtr, str_json.c_str(), EXT_TYPE_OBJ, str_json.length());
			break;
		}		


		default : {
			raise_warning("error type of data element");


		}
	}
}


void unpackElement( char **p, Variant* pout) {

	const char c = (**p);
	char * pos = *p;
	mp_type type = mp_typeof(c);
	
	switch(type) {

		case MP_NIL : {
			pout->setNull();
			(*p)++;
			break;
		}

		case MP_UINT : {
			int64_t intval = mp_decode_uint(const_cast<const char**>(&pos));
	
			*pout = intval;
			*p = pos;
			break; 
		}

		case MP_INT : {
			int64_t intval = mp_decode_int(const_cast<const char**>(&pos));
	
			*pout = intval;
			*p = pos;
			break; 
		}

		case MP_STR : {
			uint32_t len = 0;
			const char * res = mp_decode_str(const_cast<const char**>(&pos), &len);
			*p = pos;
			*pout = String(StringData::Make( res, len, CopyString ));
			break;
		}

		case MP_BIN : {
			uint32_t len = 0;
			const char * res = mp_decode_bin(const_cast<const char**>(&pos), &len);
			*p = pos;
			*pout = String(StringData::Make( res, len, CopyString ));
			break;
		}

		case MP_BOOL : {
			const bool res = mp_decode_bool(const_cast<const char**>(&pos));
			*p = pos;
			*pout =  res ? true : false;
			break;

		}

		case MP_DOUBLE : {
			double dbl_val = mp_decode_double(const_cast<const char**>(&pos));
			*pout = dbl_val;
			*p = pos;
			break; 
		}

		case MP_ARRAY : {
			int count = mp_decode_array(const_cast<const char**>(&pos));
			Array ret = Array::Create();
			for(int i = 0; i < count; ++i) {
				Variant val;
				unpackElement( &pos, &val);
				ret.add(i,val);
			}

			*pout = ret;
			*p = pos;
			break; 
		}

		case MP_MAP : {
			int count = mp_decode_map(const_cast<const char**>(&pos));

			Array ret = Array::Create();
			for(int i = 0; i < count; ++i) {
				Variant key, val;
				unpackElement( &pos, &key);
				unpackElement( &pos, &val);

				ret.add(key,val,true);
			}

			*pout = ret;
			*p = pos;
			break; 
		}

		case MP_EXT : {
			uint32_t len = 0;
			uint32_t  type = 0;
			const char * res =  mp_decode_ext(const_cast<const char**>(&pos), &len, &type );			
			
			if (type == EXT_TYPE_OBJ) {
				VariableUnserializer vu(res, len, VariableUnserializer::Type::Serialize);
				*pout = vu.unserialize();
			} else {
				raise_warning("unpack unsupport ext type");	
			}

			*p = pos;
			break;
		}

		default :  
			raise_warning("unpack error data element");
			pout->setNull();	
			mp_next(const_cast<const char**>(p));
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////

void MsgpackExtension::moduleInit() {
	
	HHVM_FE(msgpack_pack);
	HHVM_FE(msgpack_unpack);
	HHVM_FE(msgpack_set_options);


	loadSystemlib();

	MsgpackExtension::BufferSize = BUFFSIZE;
	MsgpackExtension::Buffer = malloc(BUFFSIZE);	
	MsgpackExtension::Options = 0;
}

void MsgpackExtension::moduleShutdown() {

	free(MsgpackExtension::Buffer);
	MsgpackExtension::Buffer = NULL;
}


//////////////////    static    /////////////////////////
int MsgpackExtension::BufferSize = 0;
void* MsgpackExtension::Buffer = NULL;
char* MsgpackExtension::BufferPtr = NULL;



static MsgpackExtension s_msgpack_extension;



//////////////////    HHVM_FUNCTION     //////////////////


static void HHVM_FUNCTION(msgpack_unpack, int64_t optons) {
	printf("set_options %d\n", optons );
}

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {

	// the find package len and округляем до кратного BUFFSIZE
	int new_len = BUFFSIZE * ceil( sizeof_pack(data) / static_cast<float>(BUFFSIZE) ); 

	if (new_len > MsgpackExtension::BufferSize) {
		free(MsgpackExtension::Buffer);
		MsgpackExtension::BufferSize = new_len;
		MsgpackExtension::Buffer = malloc(new_len);

		if (MsgpackExtension::Buffer == NULL) {
			raise_error("not enough memory");
		}

	}

	MsgpackExtension::BufferPtr = static_cast<char*>(MsgpackExtension::Buffer);
	
	for (int i = 0; i < data.length(); ++i)	{
		packVariant(data[i]);
	}

	size_t len = static_cast<size_t>(reinterpret_cast<uint64_t>(MsgpackExtension::BufferPtr) - reinterpret_cast<uint64_t>(MsgpackExtension::Buffer));

	StringData* str =  StringData::Make(reinterpret_cast<const char*>(MsgpackExtension::Buffer), len, CopyString);

	return  String(str);
}


static Array HHVM_FUNCTION(msgpack_unpack, const String& data) {
	Array ret = Array::Create();

	char * p = const_cast<char *>(data.c_str());
	int len = 0;
	int i=0;

	char * p0;

	p0 = p;
	while(len < data.length() ) {		
		Variant el;
		unpackElement(&p, &el);
		ret.set(i++, el);
		len +=  abs(reinterpret_cast<int64_t>(p0) - reinterpret_cast<int64_t>(p));
		p0 = p;
	}

	return  ret;
}



HHVM_GET_MODULE(msgpack);

} // namespace HPHP