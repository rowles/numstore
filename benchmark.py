import numstore
import pandas as pd
import numpy as np
import zarr

from time import time
from pathlib import Path


def file_size(path):
    path = Path(path)

    if path.is_file():
        size = path.stat().st_size
    else:
        size = sum(f.stat().st_size for f in path.glob('**/*') if f.is_file())

    return size


def timereps(reps, func):
    start = time()
    for i in range(0, reps):
        func()
    end = time()
    return ((end - start) / reps)*1e+6


def run_benchmark(arr, reps=5):
    df = pd.DataFrame(arr, columns=['value'])

    def write_numstore():
        w = numstore.Writer("TurboPForV")
        w.write("test.vec".encode(), arr)
        w.close()
    
    def write_parquet():
        df.to_parquet('test.parquet')
    
    def write_zarr():
        zarr.save('test.zarr', arr)
    
    def read_numstore():
        with numstore.Reader("test.vec".encode()) as w:
            _ = w.read()
    
    def read_parquet():
        pd.read_parquet('test.parquet')
    
    def read_zarr():
        _ = zarr.load('test.zarr')
    
    def timereps(reps, func):
        start = time()
        for i in range(0, reps):
            func()
        end = time()
        return ((end - start) / reps)*1e+3 # ms
    
    results = [
        (write_parquet, 'test.parquet'),
        (write_numstore, 'test.vec'),
        (write_zarr, 'test.zarr'),
        (read_parquet, 'test.parquet'),
        (read_numstore, 'test.vec'),
        (read_zarr, 'test.zarr'),
    ]

    for funct, file_name in results:
        timing = timereps(reps, funct)
        mb = file_size(file_name)

        timing_str = f'{timing:.2f}'.rjust(8)
        mb_str = f'{mb/(1024**2):.2f}'.rjust(8)
        output_str = f'{funct.__name__.ljust(30)} {timing_str} ms {mb_str} MB'
        print(output_str)


def gen_zipf(n):
    data = np.random.zipf(1.5, n)
    return np.array(data, dtype=np.uint64)


def gen_seq(n):
    return np.array(range(n), dtype=np.uint64)


def gen_norm(n):
    data = np.random.normal(100_000, 100, n)
    return np.array(data, dtype=np.uint64)


if __name__ == '__main__':
    sizes = [1_000, 10_000, 100_000, 1_000_000, 10_000_000, 100_000_000]

    for s in sizes:
        print('-'*30)
        print(f'Zipf {s} uint64')
        arr = gen_zipf(s)
        run_benchmark(arr)

    for s in sizes:
        print('-'*30)
        print(f'Sequential {s} uint64')
        arr = gen_seq(s)
        run_benchmark(arr)

    for s in sizes:
        print('-'*30)
        print(f'Normal {s} uint64')
        arr = gen_norm(s)
        run_benchmark(arr)

