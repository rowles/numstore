import numpy as np
import numstore
import time

arr = np.array([111, 55, 11], dtype=np.uint64)

w = numstore.Writer("wtf")

w.write("wtf".encode(), arr)

N = 10_000_000
arr = np.array(list(range(N)), dtype=np.uint64)


s = time.time()
w.write("big".encode(), arr)
e = time.time()

print('big write', e-s)


r = numstore.Reader('big'.encode())
s = time.time()
r.read()
e = time.time()

print('big read', e-s)


