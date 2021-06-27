import numstore
import pandas as pd
import numpy as np
import zarr
import random

from numcodecs import blosc
blosc.set_nthreads(1)

ns = numstore.PyZReader(1)

n = 10000000
#data = [i for i in range(n)]
data = [random.randint(0, 1000) for i in range(n)]

df = pd.DataFrame(data, columns=['a'])

arr = np.array(data, dtype=np.int32)
print(arr.dtype)


def write_numstore():
    ns.write(data)

def write_parquet():
    df.to_parquet('test.parquet')

def write_zarr():
    zarr.save('test.zarr', arr, chunks=n)

def read_numstore():
    ns.read()

def read_parquet():
    pd.read_parquet('test.parquet')

def read_zarr():
    zarr.load('test.zarr')

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

