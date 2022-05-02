# OMPT doesn't seem to be working properly with clang-15
# We will only be using clang-12 for now, until we inverstigate further about compiler compatibilities

PALIAL_TRACE_LIB=../palial_trace/libpalial_trace.so
LLVM_OMP_LIB=home/xps/Work/llvm-project/build-openmp

CC=clang-12
CXX=clang++-12

FLAGS_OPENMP?=-fopenmp

CFLAGS+=-std=gnu99 -ggdb -rdynamic -ldl 
CXXFLAGS+=-stdc++11 -g

OPENMP_LIBRARY=-lomp -I/$(LLVM_OMP_LIB)/include -L/$(LLVM_OMP_LIB)/lib
#FLAGS_OPENMP+=$(OPENMP_LIBRARY)

LAPACKE_LIB=$(shell pkg-config --libs openblas) $(shell pkg-config --libs lapacke)


all: palial cholesky

palial: main.c $(wildcard ../include/*.h ../src/*.h ../src/srcqr/*.h ../src/srclu/*.h)
	$(CC) $(CFLAGS) $(FLAGS_OPENMP) $< -o $@ $(OPENMP_LIBRARY) $(LAPACKE_LIB)

cholesky: palial
	LD_PRELOAD=/$(LLVM_OMP_LIB)/lib/libomp.so:$(PALIAL_TRACE_LIB) ./palial -a lu -m 256 -b 64

.PHONY: all pal palial clean

clean:
	$(RM) palial *~

