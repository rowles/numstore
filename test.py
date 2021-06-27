import numstore
import os
import random
import time
import numpy

r = numstore.PyZReader(0)
n = 1000000
header_size = 16
#data = [random.randint(0, 10000) for i in range(n)]
#data = numpy.random.zipf(a=1.5, size=n)
data = numpy.random.randint(low=1, high=1000000, size=n)
bytes_per_int = 4
s = time.time()
r.write(data)
e = time.time()

b = os.path.getsize('test.vec') - header_size
print(f'''
n: {n}
time: {(e-s)*1000}
bytes: {b} ({b/(1024**2)} mb)
bytes/int: {b/n}
bits/int: {b*8/n}
mem: {n*bytes_per_int} ({n*bytes_per_int/(1024**2)} mb)
ratio: {b/(n*bytes_per_int)}
''')
