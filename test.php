<?php
// print_r( get_loaded_extensions());

$ob = new stdClass(); 


$arr = array(  'ob' => $ob , 'xx'=> 123,  1 => "sss", "123" );

$sting = "string";
$int = 123;
$data = msgpack_pack( [1, "xxxx", [1, 4, "sssss" , 1.5, $ob], 1.5 , $sting, $int ] );

// $data = msgpack_pack( $arr);
// var_dump($data);


// $data = msgpack_unpack( "..." );
// var_dump($data);