#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

namespace HPHP {


const StaticString 
	TMP_XX("***");

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {


	g_context->write("len=");
	g_context->write( data.length());
	g_context->write( "\n");

	for (int i=0; i < data.length(); i++) {
		Variant el(data[i]);
		g_context->write( "[");
		g_context->write( String(i) );	
		g_context->write( "]  ");
		switch(el.getType()) {
			case KindOfInt64 : g_context->write( "int\n"); break;
			case KindOfStaticString : g_context->write( "static str\n"); break;
			case KindOfString : g_context->write( "str\n"); break;
			case KindOfArray : g_context->write( "array\n"); break; 
			case KindOfObject : g_context->write( "mixed\n");break;
			case KindOfRef : g_context->write( "ref\n");break;
			 case KindOfDouble : g_context->write( "float\n");break;
			default : g_context->write( "wrong\n");
		}
		

	}


	  return  TMP_XX.get();
}

static Array HHVM_FUNCTION(msgpack_unpack, const String& data) {
	Array ret = Array::Create();
	return  ret;
}

class MsgpackExtension : public Extension {
	public:
	MsgpackExtension(): Extension("msgpack", "1.0") {}
		
		void moduleInit() override {

			HHVM_FE(msgpack_pack);
			HHVM_FE(msgpack_unpack);

			loadSystemlib();

		}
	
} s_msgpack_extension;

HHVM_GET_MODULE(msgpack);

} // namespace HPHP