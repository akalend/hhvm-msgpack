<?php
// print_r( get_loaded_extensions());


// $arr = array(  'ob' => $ob , 'xx'=> 123,  1 => "sss", "123" );

$sting = "string";
$int = 123;

// $data = msgpack_pack( [1, "xxxx", [1, 4, "sssss" , 1.5, $ob], 1.5 , $sting, $int ] );

$a = "aaaa";
$arr = [ 'key' => 'sdsd', 'klisma' =>21 ,  1  => "c"  , 2  => "d"  ,   $a => [1,2,3, null, '****'] ,  3.6  => 'z'] ;

if (true) {
// var_dump($arr);
	$data = [ 7, 256,1024, -16, 'sdsdsd', 876675];
	var_dump($data); 
	$data = msgpack_pack( $data ); //, '****', ['s'=>'dddd','apple'=>'banana', 'xx' =>21 ] 
	file_put_contents("data.bin", $data);
	// exit;
}

// $data = msgpack_pack( $arr);


 // $data = file_get_contents("data.bin");


// echo "len=",strlen($data),PHP_EOL,$data,PHP_EOL;
// $data = msgpack_cck($data);

$data = msgpack_unpack( $data);
// var_dump($data);
var_dump($data); 