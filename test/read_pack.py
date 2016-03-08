import os
import msgpack

# f = open("/tmp/data.bin", "r")
f = open("data.bin", "r")

package = f.read(1024)
f.close()

data = msgpack.unpackb(package)
print data