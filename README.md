# MessagePack HHVM module 

MessagePack (msgpack.org) is an efficient binary serialization format, which lets you exchange data among multiple languages like JSON, except that it's faster and smaller. Small integers are encoded into a single byte while typical short strings require only one extra byte in addition to the strings themselves.

It has not any dependencies.  

Support HHVM with 3.12 version. Tested on 3.12ß version.

### Installation

	
	$ git clone git@github.com:akalend/hhvm-msgpack.git
	$ cd hhvm-msgpack
	$ hphpize && cmake . && make
	$ sudo make install

edit the hhvm config files: /etc/hhvm/php.ini 

	hhvm.dynamic_extension_path =  /usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/
	hhvm.dynamic_extensions[] = msgpack.so

### Limitation
	
Root element must be array, but is packed as sequence

Support Ext (msqpack specification ext type) only for user type = 5 as standart object serialization:

		$ob = new stdClass();
		$ob->a = 1;
		$ob->b = true;
		$data = msgpack_pack( [$ob] ); // root element must be array
  		// ...
  		$ob = msgpack_unpack($data);
  		var_dump($ob[0]);

		 object(stdClass)#4 (2) {
		    ["a"]=>
		    int(1)
		    ["b"]=>
		    bool(true)
		}


The Hack collections packed: 

		$v = Vector{42,777};
		$m = Map{'hello' => 'work'};
		$data = [$v, $m ];
		$packed = msgpack_pack( $data ); // the $m & $v will be packed as array
		msgpack_unpack( $packed );  	 // the $m & $v will be unpacked as array

### Example

	$data = [ 1, "***", ['foo'=>'bar', 'Alice' => 'Bod', 'Black' => 'Withe' ] , 1.6 , null ]  // the data must be array
	$encode_data = msgpack_pack( $data );
	...
	$data = msgpack_unpack( $dencode_data);

### Test

The run test: 

		test/test.php
							serialize/unserialize
		-----------------------------------
		nil len=1/1		 			Ok/Ok
		false len=1/1		 		Ok/Ok
		true len=1/1		 		Ok/Ok
		7-bit uint #1 len=1/1		 Ok/Ok
		7-bit uint #2 len=1/1		 Ok/Ok
		7-bit uint #3 len=1/1		 Ok/Ok
		5-bit sint #1 len=1/1		 Ok/Ok
		...
		16-bit map #2 len=261761/261761	Ok/Ok
		32-bit map len=261769/261769	Ok/Ok
		complex map len=40/40		 	Ok/Ok
### Authors

Alexandre Kalendarev - initial author

Roman Tsisyk [msg-packer https://github.com/rtsisyk/msgpuck]

Bjoern Hoehrmann [utf-8 detect  http://bjoern.hoehrmann.de/utf-8/decoder/dfa/] 

Eugene Leonovich [msgpack tests https://github.com/rybakit/msgpack.php/tree/master/tests]

