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

	 part this code was derived from tarantool/msgpuck, author Roman Tsisyk

	 for HHVM version 3.13
*/

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

#define MP_SOURCE 1
#include "msgpuck.h"
#include "msgpack.h"

namespace HPHP {


/*

								 //       |||
  KindOfUninit          = 0x00,  //  00000000    0
  KindOfNull            = 0x01,  //  00000001    1
  KindOfBoolean         = 0x09,  //  00001001   11
  KindOfInt64           = 0x11,  //  00010001 	17
  KindOfDouble          = 0x19,  //  00011001	25
  KindOfPersistentString = 0x1b, //  00011011	27
  KindOfPersistentArray = 0x1d,  //  00011101	29
  KindOfString          = 0x22,  //  00100010	34
  KindOfArray           = 0x34,  //  00110100	52
  KindOfObject          = 0x40,  //  01000000	64


*/


static void printVariant(const Variant& data);
static void packVariant(const Variant& el);
static int sizeof_pack(const Array& data);
static int sizeof_el(const Variant& el);

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

static int sizeof_array(const Array& arr, bool isMap) {
	ArrayData* data = arr.get();
	int len = 0;
	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
			pos = data->iter_advance(pos)) {
			const Variant val = data->getValue(pos);
			int keylen = 0;
			
			const Variant key = data->getKey(pos);
			if (isMap) {				
				keylen = sizeof_el(key);
			}

			int ll = sizeof_el(val);
			len += ll + keylen;
			// printf("val [%d] len=%d + %d  typ=%d/%d\n", len,ll, keylen, val.getType(), key.getType());

	}

	// printf("return %d\n", len);
	return len;
}


static int sizeof_el( const Variant& el ) {
	int size = 0;
			switch(el.getType()) {

				case KindOfInt64 : {
					size += mp_sizeof_int( el.toInt64() );
					break;
				}

				case KindOfNull : {
					size += mp_sizeof_nil();
					break;
				}

				case KindOfPersistentString:
				case KindOfString : {

					size += mp_sizeof_str( el.toString().length());
					break;
				}

				case KindOfPersistentArray :
				case KindOfArray : {

					bool isMap = checkIsMap(el.toArray());
					if (isMap) {
						size += mp_sizeof_map( el.toArray().size() );
						size += sizeof_array(el.toArray(), isMap);

					} else {
						size += mp_sizeof_array(el.toArray().size());
						size += sizeof_array( el.toArray(),isMap);
					}
					break;
				}

				case KindOfDouble :
					size += mp_sizeof_float(el.toDouble());
					break;

				case KindOfBoolean :
					size += mp_sizeof_bool(el.toBoolean());
					break;


				default : printf(" wrong type %d\n", el.getType() );
			}

			// printf("el=%d len=%d\n", el.getType(), size);
	return size;
}

static int sizeof_pack( const Array& data ) {
		
	int size = 0;

	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
		pos = data->iter_advance(pos)) {
		const Variant key = data->getKey(pos);
		const Variant val = data->getValue(pos);
		
		size += sizeof_el(val) + mp_sizeof_int(key.toInt64());
		// printf("iterate size %d\n",size );
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
		
		default : raise_warning("error type of data element");
					// printf("type is %d\n", el.getType());
	}
};

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

	loadSystemlib();

	MsgpackExtension::BufferSize = BUFFSIZE;
	MsgpackExtension::Buffer = malloc(BUFFSIZE);	

}

void MsgpackExtension::moduleShutdown() {

	// printf("moduleShutdown size=%d\n", MsgpackExtension::BufferSize);

	free(MsgpackExtension::Buffer);
	MsgpackExtension::Buffer = NULL;
}


//////////////////    static    /////////////////////////
int MsgpackExtension::BufferSize = 0;
void* MsgpackExtension::Buffer = NULL;
char* MsgpackExtension::BufferPtr = NULL;



static MsgpackExtension s_msgpack_extension;



//////////////////    HHVM_FUNCTION     //////////////////


static String HHVM_FUNCTION(msgpack_pack, const Array& data) {

	// тут надо найти длинну пакета и выделить под него буфер
	int pkg_len = sizeof_pack(data);
	// printf("package is len %d\n", pkg_len);

	if (pkg_len > MsgpackExtension::BufferSize) {
		free(MsgpackExtension::Buffer);
		MsgpackExtension::BufferSize = pkg_len * 3;
		MsgpackExtension::Buffer = malloc(MsgpackExtension::BufferSize);
		printf("***** reallock to %d\n", MsgpackExtension::BufferSize);

		if (MsgpackExtension::Buffer == NULL) {
			raise_error("not engort memory");
		}

	}

	MsgpackExtension::BufferPtr = static_cast<char*>(MsgpackExtension::Buffer);
	//MsgpackExtension::BufferPtr = mp_encode_array( MsgpackExtension::BufferPtr, data.length());	
	
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
	// char c = *p;
	int len = 0;
	int i=0;

	char * p0;


	printf("parse len=%d\n", data.length());
	p0 = p;
	while(len < data.length() ) {
		len +=  abs((long int )p0 - (long int )p);
		printf("iter: len=%d\n", len);
		Variant el;
		unpackElement(&p, &el);
		ret.set(i++, el);
	}

	//int count = mp_decode_array( const_cast<const char**>(&p));

	// printf("elements %d\n", count);

	// for (int i =0; i < count; i++) {
	// 	Variant el;
	// 	unpackElement(&p, &el);
	// 	ret.set(i, el);
	// }

	return  ret;
}



HHVM_GET_MODULE(msgpack);

} // namespace HPHP