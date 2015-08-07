#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

#define MP_SOURCE 1
#include "msgpuck.h"
#include "msgpack.h"

namespace HPHP {

#define BUFFSIZE 1024

const StaticString 
	TMP_XX("***");



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


static void printVariant(const Variant& data);

static void printElement(const Variant& key, const Variant& val) {
	printVariant(key);
	printVariant(val);
	g_context->write( "\n");
}


static void packVariant(const Variant& el){
	
};

static void packElement(const Variant& key, const Variant& val) {
	

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
		 
			default : g_context->write( "wrong");
		}
}




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
void* MsgpackExtension::Buffer = NULL;
void* MsgpackExtension::BufferPtr = NULL;

static MsgpackExtension s_msgpack_extension;

//////////////////    HHVM_FUNCTION     //////////////////


static String HHVM_FUNCTION(msgpack_check, const Array& data) {

	ArrayData* ad = data.get();
  	arrayIteration(ad, printElement);

	return  TMP_XX.get();
}

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {
	
	// int size = sizeof_pack(data);
	// printf("buff = %d, size=%d\n\n", MsgpackExtension::BufferSize, size);
	// MsgpackExtension::BufferSize = size;
	
	MsgpackExtension::BufferPtr = MsgpackExtension::Buffer;
	ArrayData* ad = data.get();
  	arrayIteration(ad, packElement);



	return  TMP_XX.get();
}


static Array HHVM_FUNCTION(msgpack_unpack, const String& data) {
	Array ret = Array::Create();
	return  ret;
}



HHVM_GET_MODULE(msgpack);

} // namespace HPHP