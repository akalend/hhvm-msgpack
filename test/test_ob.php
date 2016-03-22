
n test($type, $variable, $object, $result = null)
{
    $serialized = msgpack_pack($variable);
    $unserialized = msgpack_unpack($serialized, $object);
    var_dump($unserialized);
    if ($result)
    {
        echo $unserialized == $result ? 'OK' : 'ERROR', PHP_EOL;
    }
    else
    {
        echo 'SKIP', PHP_EOL;
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
test('array(1, 2, 3, 4)', array(1, 2, 3, 4), 'Obj');
test('array("foo", "foobar", "foohoge")', array("foo", "foobar", "hoge"), 'Obj', new Obj("foo", "foobar", "hoge"));
test('array("a" => 1, "b" => 2))', array("a" => 1, "b" => 2), 'Obj', new Obj(1, 2, null));
test('array("one" => 1, "two" => 2))', array("one" => 1, "two" => 2), 'Obj', new Obj(null, null, null, array("one" => 1, "two" => 2)));
test('array("a" => 1, "b" => 2, 3))', array("a" => 1, "b" => 2, 3), 'Obj', new Obj(1, 2, 3));
test('array(3, "a" => 1, "b" => 2))', array(3, "a" => 1, "b" => 2), 'Obj', new Obj(1, 2, 3));
test('array("a" => 1, 3, "b" => 2))', array("a" => 1, 3, "b" => 2), 'Obj', new Obj(1, 2, 3));

