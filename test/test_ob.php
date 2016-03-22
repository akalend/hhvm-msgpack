<?php
function test($type, $variable, $object, $result = null)
{
    
    if (is_array($variable)) {
        if (count($variable)==3) {
            $ob = new Obj( isset($variable[0]) ? $variable[0] : null, isset($variable[1]) ? $variable[1] : null, isset($variable[2]) ? $variable[2] : null );
        } else if (count($variable)==4) {
            $ob = new Obj( isset($variable[0]) ? $variable[0] : null, isset($variable[1]) ? $variable[1] : null, isset($variable[2]) ? $variable[2] : null , isset($variable[3]) ? $variable[3] : null );
        } else if (count($variable)==2) {
            $ob = new Obj( isset($variable[0]) ? $variable[0] : null, isset($variable[1]) ? $variable[1] : null );
        } else {
            echo "SKIP ", $type, " count error [", count($variable), "]\n";
            return;
        }
    } else {
        $ob = new Obj( $variable ); 
    }



    $serialized = msgpack_pack([$ob]);
    $unserialized = msgpack_unpack($serialized);
    $unserialized = $unserialized[0];

    // var_dump($unserialized);
    if ($result)
    {
        if ($unserialized == $result) {
            echo $type , "\t\t\tOK\n";
        } else{
            echo $type , "\t\t\t", $unserialized == $result ? 'OK' : 'ERROR', PHP_EOL;
            // var_dump($unserialized);
        }
    }
    else
    {
        echo $type , "\t\t\t",'SKIP', PHP_EOL;
    }
}
class Obj
{
    public $a;
    protected $b;
    private $c;
    public function __construct($a = null, $b = null, $c = null, $d = null)
    {
        $this->a = $a;
        $this->b = $b;
        $this->c = $c;
        if (is_array($d))
        {
            foreach ($d as $key => $val)
            {
                $this->{$key} = $val;
            }
        }
    }
}
test('null', null, 'Obj', new Obj(null, null, null));
test('bool: true', true, 'Obj', new Obj(true, null, null));
test('bool: false', false, 'Obj', new Obj(false, null, null));
test('zero: 0', 0, 'Obj', new Obj(0, null, null));
test('small: 1', 1, 'Obj', new Obj(1, null, null));
test('small: -1', -1, 'Obj', new Obj(-1, null, null));
test('medium: 1000', 1000, 'Obj', new Obj(1000, null, null));
test('medium: -1000', -1000, 'Obj', new Obj(-1000, null, null));
test('large: 100000', 100000, 'Obj', new Obj(100000, null, null));
test('large: -100000', -100000, 'Obj', new Obj(-100000, null, null));
test('double: 123.456', 123.456, 'Obj', new Obj(123.456, null, null));
test('empty: ""', "", 'Obj', new Obj("", null, null));
test('string: "foobar"', "foobar", 'Obj', new Obj("foobar", null, null));

test('array: empty', array(), 'Obj', new Obj(null, null, null));
test('array(1, 2, 3)', array(1, 2, 3), 'Obj', new Obj(1, 2, 3));
test('array(array(1, 2, 3), arr...', array(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9)), 'Obj', new Obj(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9)));
test('array(1, 2, 3, 4)', array(1, 2, 3, 4), 'Obj', new Obj(1,2,3,4));
test('array(1, 2, 3, [1,2,3,4])', array(1, 2, 3, [1,2,3,4]) , 'Obj', new Obj(1,2,3,[1,2,3,4]));
test('array("foo", "foobar", "foohoge")', array("foo", "foobar", "hoge"), 'Obj', new Obj("foo", "foobar", "hoge"));

$a = array_fill(0, 10, 1);
test('array(1, 2, 3, [1,1,... (10) ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));

$a = array_fill(0, 512, 1);
test('array(1, 2, 3, [1,1,... (512) ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));

$a = array_fill(0, 4096, 1);
test('array(1, 2, 3, [1,1,... (4096) ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));

$dim = 4438;
$a = array_fill(0, $dim, 1);
test('array(1, 2, 3, [1,1,... ('.$dim.') ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));

$dim = 4439;
$a = array_fill(0, $dim, 1);
test('array(1, 2, 3, [1,1,... ('.$dim.') ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));

$a = array_fill(0, 8096, 1);
test('array(1, 2, 3, [1,1,... (8096) ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));


$a = array_fill(0, 16385, 1);
test('array(1, 2, 3, [1,1,... (16385) ])', array(1, 2, 3, $a) , 'Obj', new Obj(1,2,3,$a));


