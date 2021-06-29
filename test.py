import numpy as np
import numstore
import time


def test_small():
    arr = np.array([111, 55, 11], dtype=np.uint64)

    w = numstore.Writer("data/small.vec")
    w.write("data/small.vec".encode(), arr)
    w.close()

    r = numstore.Reader('data/small.vec'.encode())
    arr2 = r.read()
    r.close()

    assert arr.all() == arr2.all()


def test_big():
    N = 5_000_000
    arr = np.array(range(N), dtype=np.uint64)

    w = numstore.Writer("data/big.vec")
    w.write("data/big.vec".encode(), arr)
    w.close()
    
    r = numstore.Reader('data/big.vec'.encode())
    arr2 = r.read()
    r.close()
    
    assert arr.all() == arr2.all()


def test_context():
    arr = np.array([1, 2, 3], dtype=np.uint64)

    with numstore.Writer("data/small2.vec") as w:
        w.write("data/small2.vec".encode(), arr)

    with numstore.Reader('data/small2.vec'.encode()) as r:
        arr2 = r.read()

    assert arr.all() == arr2.all()

