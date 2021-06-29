import numstore
import pandas as pd
import numpy as np
import zarr
import random

from numcodecs import blosc
#blosc.set_nthreads(1)


n = 10_000_000
#data = [i for i in range(n)]
#data = [random.randint(0, 1000) for i in range(n)]
data = np.random.zipf(1.5, n)
df = pd.DataFrame(data, columns=['a'])

arr = np.array(data, dtype=np.uint64)
print(arr)


def write_numstore():
    w = numstore.Writer("test.vec")
    w.write("test.vec".encode(), arr)
    w.close()

def write_parquet():
    df.to_parquet('test.parquet')

def write_zarr():
    zarr.save('test.zarr', arr)

def read_numstore():
    with numstore.Reader("test.vec".encode()) as w:
        _ = w.read()
        np.sum(_)

def read_parquet():
    pd.read_parquet('test.parquet')

def read_zarr():
    _ = zarr.load('test.zarr')
    np.sum(_)

def timereps(reps, func):
    from time import time
    start = time()
    for i in range(0, reps):
        func()
    end = time()
    return ((end - start) / reps)*1e+6
import timeit

reps = 5
print('write_parquet', timereps(reps, write_parquet))
print('write_numstore', timereps(reps, write_numstore))
print('write_zarr', timereps(reps, write_zarr))

print('read_parquet', timereps(reps, read_parquet))
print('read_numstore', timereps(reps, read_numstore))
print('read_zarr', timereps(reps, read_zarr))

