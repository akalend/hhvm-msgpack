# MessagePack HHVM module 

MessagePack (msgpack.org) is an efficient binary serialization format, which lets you exchange data among multiple languages like JSON, except that it's faster and smaller. Small integers are encoded into a single byte while typical short strings require only one extra byte in addition to the strings themselves.

For pack/unpack use the Roman Tsisyk msg-packer https://github.com/rtsisyk/msgpuck

It has not any dependencies.

### Installation

	
	$ git clone git@github.com:akalend/hhvm-msgpack.git
	$ cd hhvm-msgpack
	$ hphpize && cmake . && make
	$ sudo make install

edit the hhvm config files: /etc/hhvm/php.ini 

	hhvm.dynamic_extension_path =  /usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/
	hhvm.dynamic_extensions[] = msgpack.so
 



### Usage

	$encode_data = msgpack_pack( $data );
	...
	$data = msgpack_unpack( $dencode_data);

### This is alpha version
