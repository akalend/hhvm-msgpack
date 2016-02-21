# MessagePack HHVM module 

MessagePack (msgpack.org) is an efficient binary serialization format, which lets you exchange data among multiple languages like JSON, except that it's faster and smaller. Small integers are encoded into a single byte while typical short strings require only one extra byte in addition to the strings themselves.

It has not any dependencies.  

Support HHVM with 3.9 version. Tested on 3.14 version.

### Installation

	
	$ git clone git@github.com:akalend/hhvm-msgpack.git
	$ cd hhvm-msgpack
	$ hphpize && cmake . && make
	$ sudo make install

edit the hhvm config files: /etc/hhvm/php.ini 

	hhvm.dynamic_extension_path =  /usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/
	hhvm.dynamic_extensions[] = msgpack.so

### Limitation
	
	Root element must be array

	Don't support Ext type (msqpack specification ext type)

	Don't work with built-in Hack collections (in TODO list). You must make typization to array:

		$v = Vector{42,777};
		$m = Map{'hello' => 'work'};
		$data = [1, (array)$v, (array)$m ]; // typization to array



	The common lenght of pack must be less 4096 bytes. If need more, You can change define BUFFSIZE  the line 8 in the msgpack.h (Dynamic memory allocate in TODO list):

		#define BUFFSIZE 4096



### Example

	$data = [ 1, "***", ['foo'=>'bar', ...] , 1.6 , null ]  // the data must be array
	$encode_data = msgpack_pack( $data );
	...
	$data = msgpack_unpack( $dencode_data);



### Authors

Alexandre Kalendarev - initial author

Roman Tsisyk [msg-packer https://github.com/rtsisyk/msgpuck]
