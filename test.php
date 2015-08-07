<?php
// print_r( get_loaded_extensions());


// $arr = array(  'ob' => $ob , 'xx'=> 123,  1 => "sss", "123" );

$sting = "string";
$int = 123;

// $data = msgpack_pack( [1, "xxxx", [1, 4, "sssss" , 1.5, $ob], 1.5 , $sting, $int ] );

$a = "aaaa";
$arr = [ 'key' => 'sdsd', 'klisma' =>21 ,  1  => "c"  , 2  => "d"  ,   $a => [1,2,3, null, '****'] ,  3.6  => 'z'] ;

// var_dump($arr);

$data = msgpack_check(  [1,2,3, null, '****'] );

// $data = msgpack_pack( $arr);
// var_dump($data);


// $data = msgpack_unpack( "..." );
// var_dump($data);