import numpy as np
import numstore
import time


def test_small():
    arr = np.array([111, 55, 11], dtype=np.uint64)
    arr = np.array([11, 55, 142], dtype=np.uint64)

    for algo in numstore.ALGO_STR.keys():
        w = numstore.Writer(algo, 'data/small.vec')
        w.write(arr)
        w.close()
    
        r = numstore.Reader('data/small.vec')
        arr2 = r.read()
        r.close()
    
        np.testing.assert_array_equal(arr, arr2)


def test_big():
    N = 50_000
    arr = np.array(range(N), dtype=np.uint64)

    for algo in numstore.ALGO_STR.keys():
        w = numstore.Writer(algo, 'data/big.vec')
        w.write(arr)
        w.close()
        
        r = numstore.Reader('data/big.vec')
        arr2 = r.read()
        r.close()

        np.testing.assert_array_equal(arr, arr2)


def test_context():
    arr = np.array([1, 2, 3], dtype=np.uint64)

    with numstore.Writer('TurboPFor', 'data/small2.vec') as w:
        w.write(arr)

    with numstore.Reader('data/small2.vec') as r:
        arr2 = r.read()

    np.testing.assert_array_equal(arr, arr2)

