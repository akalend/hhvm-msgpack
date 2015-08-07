#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

#define MP_SOURCE 1
#include "msgpuck.h"


namespace HPHP {

static String HHVM_FUNCTION(msgpack_check, const Array& data);
static Array HHVM_FUNCTION(msgpack_unpack, const String& data);
static String HHVM_FUNCTION(msgpack_pack, const Array& data);

class MsgpackExtension : public Extension {
	public:
	
	static int BufferSize;
	static void* Buffer;
	static char* BufferPtr;
	static int Level;
	
	MsgpackExtension(): Extension("msgpack", "0.1.0") {}

	void moduleInit() override;
	void moduleShutdown() override;

} ;



// HHVM_GET_MODULE(msgpack);

} // namespace HPHP