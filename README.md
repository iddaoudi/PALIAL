# PALIAL
PArallel LInear ALgebra routines

## Principle
* 3 dense OpenMP task-based linear algebra algorithms are implemented (```palial_src```): Cholesky, QR and LU decompositions
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
cd palial_src/
make
```

The algorithms parameters can be tweaked from ```palial_src/Makefile```, for example:
```
./palial -a qr -m 12288 -b 1024
```
