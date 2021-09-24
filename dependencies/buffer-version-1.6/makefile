
.PHONY: all

CONFIG = 

all: main

buffer.o: buffer.c buffer.h
	gcc -I. $(CONFIG) -c buffer.c -o buffer.o
buffer_test.o: buffer_test.h
	gcc -I. $(CONFIG) -c buffer_test.c -o buffer_test.o
example.o: example.c buffer.h buffer_test.h
	gcc -I. $(CONFIG) -c example.c -o example.o

main: buffer.o buffer_test.o example.o
	gcc -Wall -m64 buffer.o buffer_test.o example.o -o main.exe

.PHONY: clean

clean:
	del buffer.o buffer_test.o example.o main.exe

.PHONY: help

help:
	@echo Welcome to the BUFFERlib make targets help!
	@echo "make" for just building the static library libBUFFER.a
	@echo "make example" for just building the example executable example.exe 
	@echo "make install" for installing the library into directory
	@echo "make clean" for cleaning the created objects and executables
	@echo "make help" this help screen