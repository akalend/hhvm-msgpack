<?php
/**
* start without change php.ini   
* hhvm -d hhvm.extension_dir=/usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/  -d hhvm.extensions[]=msgpack.so    test/test.php
*
*/

$arr = [ 'Pupkin',  null,  42 , [1,5,'****'], [ 'sss' => 12, 0 => '2333' , 'fff'=> '****', 7 => null ]];


if (1) {
	$data = $arr;
	print_r($data); 
	$data = msgpack_pack( $data );

	// echo $data , PHP_EOL;
	file_put_contents('/tmp/data.bin', $data );
}

// $data = file_get_contents('../data.bin');
$data = msgpack_unpack( $data);
print_r($data); 