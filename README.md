# numstore

Command line utility to store compressed integer series.

## Usage
```
$ seq 10000 | numstore write -f ./test.vec

$ du -b ./test.vec
15584   test.vec

$ numstore read -f ./test.series
1
2
3
...
9998
9999
10000
```

## Building

```
$ make build_deps
$ make build
```
