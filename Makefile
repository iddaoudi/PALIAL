# Compiler
CC = gcc

# Flags
CFLAGS  = -Wall -ggdb3 \
          $(shell pkg-config --cflags openblas) \
		  $(shell pkg-config --cflags lapacke)
 
LIBS = $(shell pkg-config --libs openblas) $(shell pkg-config --libs lapacke)

MAIN          = main
TARGET        = main

all: $(TARGET)

$(TARGET): $(MAIN).c 
		$(CC) $(CFLAGS) -o $(TARGET) $(MAIN).c $(LIBS)

clean:
		$(RM) $(TARGET)
