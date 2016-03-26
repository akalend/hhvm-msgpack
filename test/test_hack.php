<?hh
/**
* start without change php.ini   
* hhvm -d hhvm.extension_dir=/usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/  -d hhvm.extensions[]=msgpack.so    test/test.php
*
*/

msgpack_set_options(777);

$v = Vector{'one', 2, true };
$m = Map{"one" => 1, "two" => 2};
$arr = [ 'Pupkin',  null,  42 ,  [ 'sss' => 12, 0 => '2333'  ], [7,13, 21, 33,'*****'] ];

	$ob = new stdClass();
	$ob->a = 1;
	$ob->b = true;
	$data = msgpack_pack( [$m, $v] );

$data = msgpack_unpack( $data);

$data = msgpack_pack( [(array)$m, (array)$v] );

$data = msgpack_unpack( $data);


var_dump($data);
exit;



if (1) {
	$data = [$m,  [1,2,3], $ob, 12, true];
	var_dump($data); 
	$data = msgpack_pack( $data );

	// echo $data , PHP_EOL;
	// file_put_contents('data.bin', $data );
}

// $data = file_get_contents('../data.bin');
