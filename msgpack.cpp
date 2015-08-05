#include "hphp/runtime/ext/extension.h"

namespace HPHP {

class MsgpackExtension : public Extension {
	public:
	MsgpackExtension(): Extension("msgpack", "1.0") {}
		
		void moduleInit() override {
			loadSystemlib();
		}
	
} s_msgpack_extension;

HHVM_GET_MODULE(msgpack);

} // namespace HPHP