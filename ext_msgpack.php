<?hh
/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2015 Alexandre Kalendarev                              |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

/**
*  packed the elements 
*  
*  @example  $data = msgpack_pack([42, 'Vasiliy Pupkin', true])
*
*  @param array $data  - input data
*  @return string      - packed data
*/
<<__Native>>
function msgpack_pack( array $data ): String;


/**
*  unpacked the binary data (string) 
*  
*  @example  $data = msgpack_unpack($packed_data)
*
*  @param  string $msg     - packed binary data
*  @return array           - elements
*/
<<__Native>>
function msgpack_unpack(string $msg): Array;

