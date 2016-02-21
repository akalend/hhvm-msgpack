<?hh
/**
* start without change php.ini   
* hhvm -d hhvm.extension_dir=/usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/  -d hhvm.extensions[]=msgpack.so    test/test.php
*
*/

$v = Vector{1,2,3};
$s = Set{'xxx','yyy','zzz'};
$arr = [ 'Pupkin',  (array)$v,  (array)$s , [1,5,['x'=>'****']], [ 'sss' => 12, 0 => '2333'  ]];

if (1) {
	$data = $arr;
	print_r($data); 
	$data = msgpack_pack( $data );

	// echo $data , PHP_EOL;
	// file_put_contents('data.bin', $data );
}

// $data = file_get_contents('../data.bin');
$data = msgpack_unpack( $data);
var_dump($data); 