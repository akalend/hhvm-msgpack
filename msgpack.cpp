#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

#define MP_SOURCE 1
#include "msgpuck.h"
#include "msgpack.h"

namespace HPHP {


const StaticString 
	TMP_XX("***");



static int check_type( const Array& data ) {
	
	g_context->write("len=");
	g_context->write( data.length());
	g_context->write( "\n");

	int size = 0;

	for (int i=0; i < data.length(); i++) {
		Variant el(data[i]);
		g_context->write( "[");
		g_context->write( String(i) );
		g_context->write( "]  ");
		switch(el.getType()) {
			case KindOfInt64 : { 
				g_context->write( "int\n"); 
				size += mp_sizeof_int( el.toInt64() );
				printf("int %d\n", mp_sizeof_int( el.toInt64()));
				break; }
			
			case KindOfNull : { 
				g_context->write( "null\n"); 
				size += mp_sizeof_nil(  );
				printf("null \n");
				break; }

			case KindOfStaticString : 
			case KindOfString : {
				g_context->write( "str\n"); 
				size += mp_sizeof_str( el.toString().length());
				printf("str %d\n", mp_sizeof_str( el.toString().length()));
				break;}
			case KindOfArray : g_context->write( "array\n"); {
				size += mp_sizeof_array( el.toArray().size() );
				printf("arr header: %d\n", mp_sizeof_array( el.toArray().size() ));
				int arr_size = check_type( el.toArray() );
				size += arr_size;
				printf("array size: %d\n", arr_size);

				g_context->write( "--- end array  [");
				g_context->write( String(i) );
				g_context->write( "]\n");
				break; }
			case KindOfObject : g_context->write( "mixed\n");	break;
			case KindOfRef : g_context->write( "ref\n");	break;
			case KindOfDouble : 
				size += mp_sizeof_float(el.toDouble());
				g_context->write( "float\n");
				printf("float: %d\n",  mp_sizeof_float(el.toDouble()));				
				break;
			case KindOfClass : g_context->write( "class\n");break;
			 
			default : g_context->write( "wrong\n");
		}
	}

	return size;
}


void MsgpackExtension::moduleInit() {
	
	HHVM_FE(msgpack_check);
	HHVM_FE(msgpack_pack);
	HHVM_FE(msgpack_unpack);

	loadSystemlib();

}

void MsgpackExtension::moduleShutdown() {
	printf("moduleShutdown size=%d\n", MsgpackExtension::BufferSize);
}


//////////////////    static    /////////////////////////
int MsgpackExtension::BufferSize = 0;
char* MsgpackExtension::Buffer = NULL;

static MsgpackExtension s_msgpack_extension;

//////////////////    HHVM_FUNCTION     //////////////////



// enum DataType : int8_t {
//   // Values below zero are not PHP values, but runtime-internal.
//   KindOfClass         = -13,

//   // Any code that static_asserts about the value of KindOfNull may also depend
//   // on there not being any values between KindOfUninit and KindOfNull.

//                                //      uncounted init bit
//                                //      |string bit
//                                //      ||
//   KindOfUninit        = 0x00,  //  00000000
//   KindOfNull          = 0x08,  //  00001000
//   KindOfBoolean       = 0x09,  //  00001001
//   KindOfInt64         = 0x0a,  //  00001010
//   KindOfDouble        = 0x0b,  //  00001011
//   KindOfStaticString  = 0x0c,  //  00001100
//   KindOfString        = 0x14,  //  00010100
//   KindOfArray         = 0x20,  //  00100000
//   KindOfObject        = 0x30,  //  00110000
//   KindOfResource      = 0x40,  //  01000000
//   KindOfRef           = 0x50,  //  01010000
// };

static void printVariant(const Variant& data);

static void printElement(const Variant& key, const Variant& val) {
	printVariant(key);
	printVariant(val);
	g_context->write( "\n");
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



static String HHVM_FUNCTION(msgpack_check, const Array& data) {

	ArrayData* ad = data.get();
  	arrayIteration(ad, printElement);
  	g_context->write( "----- end ----\n");

	// for (ssize_t pos = ad->iter_begin(); pos != ad->iter_end();
	//        pos = ad->iter_advance(pos)) {
	//        const Variant key = ad->getKey(pos);
	//    	   const Variant val = ad->getValue(pos);
		
	// 		printVariant( key);
	// 		g_context->write( "\t\t");
	// 		printVariant( val);
	// 		g_context->write( "\n");
	// }


	return  TMP_XX.get();
}

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {
	
	int size = check_type(data);
	printf("buff = %d, size=%d\n\n", MsgpackExtension::BufferSize, size);
	MsgpackExtension::BufferSize = size;

	return  TMP_XX.get();
}


static Array HHVM_FUNCTION(msgpack_unpack, const String& data) {
	Array ret = Array::Create();
	return  ret;
}



HHVM_GET_MODULE(msgpack);

} // namespace HPHP