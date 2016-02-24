import os
import msgpack

<<<<<<< HEAD
f = open("/tmp/data.bin", "r")
=======
f = open("data.bin", "r")
>>>>>>> develope
package = f.read(1024)
f.close()

data = msgpack.unpackb(package)
print data