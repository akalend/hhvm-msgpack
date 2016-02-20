<?php
/**
* start without change php.ini   
* hhvm -d hhvm.extension_dir=/usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/  -d hhvm.extensions[]=msgpack.so    test/test.php
*
*/

$arr = [ 'key', 'sdsd', 'klisma', 21 ,  [ 'sss' => 12, 0 => '2333'  ]];

if (0) {
	$data = $arr;
	var_dump($data); 
	$data = msgpack_pack( $data );
	file_put_contents('data.bin', $data );
}

$data = file_get_contents('../data.bin');
$data = msgpack_unpack( $data);
var_dump($data); 