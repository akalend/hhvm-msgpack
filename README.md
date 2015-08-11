# MessagePack HHVM module 

MessagePack (msgpack.org) is an efficient binary serialization format, which lets you exchange data among multiple languages like JSON, except that it's faster and smaller. Small integers are encoded into a single byte while typical short strings require only one extra byte in addition to the strings themselves.

For pack/unpack use the Roman Tsisyk msg-packer https://github.com/rtsisyk/msgpuck

Hav't the any dependency.

### Usage

	$data = [ 'key', 'sdsd', 'klisma', 21 ,   ['kkk'  => 'z', 'dsn'  => "c"  , 2 => "d"] , [1,2,3, null, '****'] ];
	var_dump($data); 
	$encode_data = msgpack_pack( $data );

	$data = msgpack_unpack( $dencode_data);
	var_dump($data); 

### This is alpha version
