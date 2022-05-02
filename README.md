# PALIAL
PArallel LInear ALgebra routines

## Principle
* 3 dense OpenMP task-based linear algebra algorithms are implemented (```palial_algs```)
* Tracing these algorithms is done with the ```palial_trace``` library using OMPT callbacks

## Compilation

### TRACE
```
cd palial_trace/
cmake .
make
```

### ALGS
```
cd palial_algs/
make
```

The algorithms parameters can be tweaked from ```palial_algs/Makefile```, for example:
```
./palial -a qr -m 12288 -b 1024
```
