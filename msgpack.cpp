#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

namespace HPHP {


const StaticString 
	TMP_XX("***");

static String HHVM_FUNCTION(msgpack_pack, const Array& data) {

	g_context->write("-----");
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