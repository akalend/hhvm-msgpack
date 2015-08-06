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
			
			case KindOfStaticString : {
				g_context->write( "static str\n"); 
				size += mp_sizeof_str( el.toString().length());
				printf("str %d\n", mp_sizeof_str( el.toString().length()));
				break;}

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

	HHVM_FE(msgpack_pack);
	HHVM_FE(msgpack_unpack);

	loadSystemlib();

}

void MsgpackExtension::moduleShutdown() {
	printf("moduleShutdown size=%d\n", MsgpackExtension::BufferSize);
}

int MsgpackExtension::BufferSize = 0;

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {

	int size = check_type(data);
	printf("buff = %d, size=%d\n", MsgpackExtension::BufferSize, size);
	MsgpackExtension::BufferSize = size;

	return  TMP_XX.get();
}


static Array HHVM_FUNCTION(msgpack_unpack, const String& data) {
	Array ret = Array::Create();
	return  ret;
}


HHVM_GET_MODULE(msgpack);

} // namespace HPHP