# numstore

Compressed integer file format using [TurboPFor](https://github.com/powturbo/TurboPFor-Integer-Compression). Currently supports TurboPFor128 and TurboPFor on uint64 vectors.

## Usage

```python
import numstore
import numpy as np

arr = np.array([1, 2, 3], dtype=np.uint64)

with numstore.Writer('TurboPFor', 'data/small.vec') as w:
    w.write(arr)

with numstore.Reader('data/small.vec') as r:
    read_arr = r.read()
```

See `benchmark.py` and `test.py` for more on usage.

## Building

Requires linux and compiler with C++20 support.

```bash
# Build TurboPFor as shared library
$ make build_ext 

# Build c++ library
$ make build_libnumstore

# Setup python virtual env
$ make setup_venv

# Build cython bindings
$ make build_cython

$ make test
$ make benchmark
```

## Benchmarks

Reads and write are single threaded. There is room for improvement with threaded reads.

```
Zipf 100000000 uint64
write_parquet                   3844.54 ms   160.09 MB
write_numstore                   915.84 ms    83.51 MB
write_zarr                       781.78 ms   112.89 MB
read_parquet                    2472.27 ms   160.09 MB
read_numstore                    850.20 ms    83.51 MB
read_zarr                        644.42 ms   112.89 MB
------------------------------
Normal 100000000 uint64
write_parquet                   2392.28 ms   121.15 MB
write_numstore                  1075.18 ms   203.42 MB
write_zarr                       719.81 ms   143.91 MB
read_parquet                    1998.51 ms   121.15 MB
read_numstore                    878.12 ms   203.42 MB
read_zarr                        672.29 ms   143.91 MB
------------------------------
Sequential 100000000 uint64
write_parquet                   3978.40 ms   382.26 MB
write_numstore                  1201.23 ms   306.63 MB
write_zarr                       340.03 ms     5.00 MB
read_parquet                    2948.92 ms   382.26 MB
read_numstore                    867.43 ms   306.63 MB
read_zarr                        550.60 ms     5.00 MB
```




