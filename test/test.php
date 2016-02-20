<?php


$arr = [ 'key', 'sdsd', 'klisma', 21 ,   ['kkk'  => 'z', 'dsn'  => "c"  , 2 => "d"] , [1,2,3, null, '****'] ];

if (1) {
	$data = $arr;
	print_r($data); 
	$data = msgpack_pack( $data );
}

$data = msgpack_unpack( $data);
print_r($data); 