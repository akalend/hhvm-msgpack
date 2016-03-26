#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"  // g_context

#define MP_SOURCE 1
#include "msgpuck.h"


#define BUFFSIZE 4096

namespace HPHP {

static Array HHVM_FUNCTION(msgpack_unpack, const String& data);
static String HHVM_FUNCTION(msgpack_pack, const Array& data);
static void HHVM_FUNCTION(msgpack_set_options, const int options);


class MsgpackExtension : public Extension {
	public:
	
	static int BufferSize;
	static void* Buffer;
	static char* BufferPtr;
	static int64_t Options;

	
	MsgpackExtension(): Extension("msgpack", "0.1.3") {}

	void moduleInit() override;
	void moduleShutdown() override;

} ;



// HHVM_GET_MODULE(msgpack);

} // namespace HPHP