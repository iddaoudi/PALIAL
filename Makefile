# OMPT doesn't seem to be working properly with clang-15
# We will only be using clang-12 for now, until we inverstigate further about compiler compatibilities
CC=clang-12
CXX=clang++-12
FLAGS_OPENMP?=-fopenmp
CFLAGS+=-std=gnu99 -ggdb 
CXXFLAGS+=-stdc++11 -g
OPENMP_LIBRARY=-lomp -I/home/xps/Work/llvm-project/build-openmp/include -L/home/xps/Work/llvm-project/build-openmp/lib
#FLAGS_OPENMP+=$(OPENMP_LIBRARY)
LAPACKE_LIB=$(shell pkg-config --libs openblas) $(shell pkg-config --libs lapacke)

######################## OMP_TOOL_LIBRARIES #######################

#all: palial libpal.so
#
#libpal.so: trace/trace.c
#	$(CC) $(CFLAGS) $(FLAGS_OPENMP) trace/trace.c -shared -fPIC -o $@
#
#palial: main.c
#	$(CC) $(CFLAGS) $(FLAGS_OPENMP) -DDYN_TOOL $< -o $@ $(LAPACKE_LIB)
#
#run-lib: palial libpal.so
#	env OMP_TOOL_LIBRARIES=$(PWD)/libpal.so ./palial -a lu -m 1024 -b 128
#
#run:run-lib
#
#.PHONY:all run-lib run clean
#
#clean:
#	$(RM) palial libpal.so *~

######################## INTEGRATED #######################

all: palial

palial: main.c $(wildcard include/*.h src/*.h src/srcqr/*.h src/srclu/*.h)
	$(CC) $(CFLAGS) $(FLAGS_OPENMP) $< -o $@ $(OPENMP_LIBRARY) $(LAPACKE_LIB)

.PHONY: all palial clean

clean:
	$(RM) palial *~

