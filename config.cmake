HHVM_EXTENSION(msgpack msgpack.cpp)
include_directories(include)

HHVM_SYSTEMLIB(msgpack ext_msgpack.php)
