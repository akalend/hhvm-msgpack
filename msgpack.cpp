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
*/


#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

#define MP_SOURCE 1
#include "msgpuck.h"
#include "msgpack.h"

namespace HPHP {

#define BUFFSIZE 1024

const StaticString 
	TMP_XX("***");


static void printVariant(const Variant& data);
static void packVariant(const Variant& el);


static int sizeof_pack( const Array& data ) {
		
	int size = 0;

	for (int i=0; i < data.length(); i++) {
		Variant el(data[i]);
		switch(el.getType()) {
			
			case KindOfInt64 : {
				size += mp_sizeof_int( el.toInt64() ); 
				break; 
			}
			
			case KindOfNull : {
				size += mp_sizeof_nil();
				break; 
			}
			
			case KindOfStaticString : 
			case KindOfString : {
				size += mp_sizeof_str( el.toString().length());
				break;
			}

			case KindOfArray : {
				size += mp_sizeof_array( el.toArray().size() );
				int arr_size = sizeof_pack( el.toArray() );
				size += arr_size;
				break; 
			}

			case KindOfDouble : 
				size += mp_sizeof_float(el.toDouble());
				break;
		 
			default : g_context->write( "wrong\n");
		}
	}

	return size;
}

static void arrayIteration(ArrayData* data, void  (arrayIterationCb) (const Variant& , const Variant&)) {

	g_context->write( "\tarray:\n");
	for (ssize_t pos = data->iter_begin(); pos != data->iter_end();
	       pos = data->iter_advance(pos)) {
	       const Variant key = data->getKey(pos);
	   	   const Variant val = data->getValue(pos);
	   	   arrayIterationCb(key, val);
	}
	g_context->write( "----- end array ----\n");

}


static void printElement(const Variant& key, const Variant& val) {
	printVariant(key);
	printVariant(val);
	g_context->write( "\n");
}

static void encodeArrayElement(const Variant& key, const Variant& val) {
	packVariant(key);
	packVariant(val); 
}


static void packVariant(const Variant& el) {
		
		switch(el.getType()) {
			case KindOfInt64 : { 
				int64_t int_val = el.toInt64();
				if (int_val >= 0)
					MsgpackExtension::BufferPtr = mp_encode_uint(MsgpackExtension::BufferPtr,  int_val );
				else
					MsgpackExtension::BufferPtr = mp_encode_int(MsgpackExtension::BufferPtr,  int_val );
				break; }
			
			case KindOfNull : { 
				MsgpackExtension::BufferPtr = mp_encode_nil(MsgpackExtension::BufferPtr);
				break; }

			case KindOfStaticString : 
			case KindOfString : {
				MsgpackExtension::BufferPtr = mp_encode_str(MsgpackExtension::BufferPtr, el.toString().c_str(), el.toString().length());
				break;
			}
			
			case KindOfArray : { 
					MsgpackExtension::BufferPtr = mp_encode_map(MsgpackExtension::BufferPtr, el.toArray().length());
					ArrayData* ad = el.toArray().get();
					arrayIteration(ad, encodeArrayElement);
					break; 
				}

			case KindOfDouble : {
				MsgpackExtension::BufferPtr = mp_encode_float(MsgpackExtension::BufferPtr, el.toDouble());
				break;
		 	}
			
			default : g_context->write( "wrong");
		}
};


static void printVariant(const Variant& data) {

		switch(data.getType()) {
			case KindOfInt64 : { 
				g_context->write( "int: "); 
				g_context->write( data.toInt64());
				break; }
			
			case KindOfNull : { 
				g_context->write( "null"); 
				break; }

			case KindOfStaticString : 
			case KindOfString : {
				g_context->write( "str: "); 
				g_context->write( data.toString()); 
				break;}
			case KindOfArray : { g_context->write( "array"); 
					ArrayData* ad = data.toArray().get();
					arrayIteration(ad, printElement);				
					break; 
				}
			case KindOfObject : g_context->write( "mixed"); break;
			case KindOfRef : g_context->write( "ref");	break;
			case KindOfDouble : 
				g_context->write( "float: ");
				g_context->write( data.toDouble());
				break;
		 
			default :  raise_warning("unpack error input data");
		}
}

// enum mp_type {
// 	MP_NIL = 0,
// 	MP_UINT,
// 	MP_INT,
// 	MP_STR=3,
// 	MP_BIN,
// 	MP_ARRAY,
// 	MP_MAP,
// 	MP_BOOL,
// 	MP_FLOAT,
// 	MP_DOUBLE,
// 	MP_EXT
// };

void unpackElement( char **p, Variant* pout) {

	const char c = (**p);
	char * pos = *p;
	mp_type type = mp_typeof(c);
	
	printf("decode type %d\n", type);

	switch(type) {

		case MP_NIL : {
			pout->setNull();
		}

		case MP_UINT : {
			int64_t intval = mp_decode_uint(const_cast<const char**>(&pos));
	
			*pout = intval;
			printf("decode int %ld\n", intval);
			*p = pos;
			break; 
		}

		case MP_INT : {
			int64_t intval = mp_decode_int(const_cast<const char**>(&pos));
	
			*pout = intval;
			printf("decode int %ld\n", intval);
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

		default :  
			// raise_warning("unpack error data element");
			printf("unpack error data element");
			pout->setNull();	
			mp_next(const_cast<const char**>(p));
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////

void MsgpackExtension::moduleInit() {
	
	HHVM_FE(msgpack_check);
	HHVM_FE(msgpack_pack);
	HHVM_FE(msgpack_unpack);

	loadSystemlib();

	MsgpackExtension::BufferSize = BUFFSIZE;
	MsgpackExtension::Buffer = malloc(BUFFSIZE);	

}

void MsgpackExtension::moduleShutdown() {

	free(MsgpackExtension::Buffer);
	printf("moduleShutdown size=%d\n", MsgpackExtension::BufferSize);
}


//////////////////    static    /////////////////////////
int MsgpackExtension::BufferSize = 0;
int MsgpackExtension::Level = 0;
void* MsgpackExtension::Buffer = NULL;
char* MsgpackExtension::BufferPtr = NULL;


static MsgpackExtension s_msgpack_extension;

//////////////////    HHVM_FUNCTION     //////////////////


static String HHVM_FUNCTION(msgpack_check, const Array& data) {

	int len = sizeof_pack(data);
	printf("len=%d\n", len);

	ArrayData* ad = data.get();
  	arrayIteration(ad, printElement);

	return  TMP_XX.get();
}

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {

	MsgpackExtension::Level = 0;	
	MsgpackExtension::BufferPtr = static_cast<char*>(MsgpackExtension::Buffer);
	MsgpackExtension::BufferPtr = mp_encode_array( MsgpackExtension::BufferPtr, data.length());	
	
	for (int i = 0; i < data.length(); ++i)	{
		packVariant(data[i]);
	}
	size_t len = reinterpret_cast<uint64_t>(MsgpackExtension::BufferPtr) - reinterpret_cast<uint64_t>(MsgpackExtension::Buffer);

	StringData* str =  StringData::Make(reinterpret_cast<const char*>(MsgpackExtension::Buffer), len, CopyString);

	return  String(str);
}


static Array HHVM_FUNCTION(msgpack_unpack, const String& data) {
	Array ret = Array::Create();

	char * p = const_cast<char *>(data.c_str());
	char c = *p;

	if ( mp_typeof(c) != MP_ARRAY ) {
 		raise_warning("the root element must be array");
		return ret;
	}
	int count = mp_decode_array( const_cast<const char**>(&p));

	printf("elements %d\n", count);

	for (int i =0; i < count; i++) {
		Variant el;
		unpackElement(&p, &el);
		ret.set(i, el);
	}

	//mp_typeof(**r)

	return  ret;
}



HHVM_GET_MODULE(msgpack);

} // namespace HPHP