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

### Todo
* more debugging for the vector size problem
* keep a single upstream function for both task names and CPU numbers and node numbers
* test real time task termination (also use bigger tile sizes) before implementing energy consumption measurement
