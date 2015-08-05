#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#define MP_SOURCE 1
#include "msgpuck.h"


// enum mp_type {
// 	MP_NIL = 0,
// 	MP_UINT,
// 	MP_INT,
// 	MP_STR,
// 	MP_BIN,
// 	MP_ARRAY,
// 	MP_MAP,
// 	MP_BOOL,
// 	MP_FLOAT,
// 	MP_DOUBLE,
// 	MP_EXT
// };


void main() {

char buf[4096];

char *w = buf;
// First MsgPack object
w = mp_encode_uint(w, 42);

// Second MsgPack object
w = mp_encode_array(w, 4);
   w = mp_encode_array(w, 2);
        // Begin of an inner array
        w = mp_encode_str(w, "second inner 1", 14);
        w = mp_encode_str(w, "second inner 2", 14);
        // End of an inner array
   w = mp_encode_str(w, "second", 6);
   w = mp_encode_uint(w, 20);
   w = mp_encode_bool(w, true);
// Third MsgPack object
w = mp_encode_str(w, "third", 5);
// EOF
const char *r = buf;
// First MsgPack object
char c = *r;

int type = mp_typeof(c) ;

uint first = mp_decode_uint(&r);
printf("type=%d uint=%d data=%d\n", type, MP_UINT, first);

 // mp_next(&r); // skip the first object

	c = *r;
// // Second MsgPack object
	assert(mp_typeof(c) == MP_ARRAY);
	uint32_t size_arr = mp_decode_array(&r);
    printf("size array=%d\n", size_arr);
	
	// mp_decode_array(&r);    
	
	c = *r;
    assert(mp_typeof(c) == MP_ARRAY); // inner array    
    

    mp_next(&r); // -->> skip the entire inner array (with all members)
    // size_arr = mp_decode_array(&r);
    // printf("size inner_array=%d\n", size_arr);

    c = *r;
    assert(mp_typeof(c) == MP_STR); // second
    mp_next(&r);
    c = *r;
    assert(mp_typeof(c) == MP_UINT); // 20
    uint el_uint = mp_decode_uint(&r);
	printf("elm=%d\n", el_uint);    
    // mp_next(&r);
    c = *r;
    assert(mp_typeof(c) == MP_BOOL); // true
    mp_next(&r);
// Third MsgPack object
    c = *r;
assert(mp_typeof(c) == MP_STR); // third
mp_next(&r);
assert(r == w); // EOF



}