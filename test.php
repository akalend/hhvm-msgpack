<?php
// print_r( get_loaded_extensions());

$data = msgpack_pack( [1,3,2] );
var_dump($data);


$data = msgpack_unpack( "..." );
var_dump($data);