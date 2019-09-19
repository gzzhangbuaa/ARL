#!/bin/bash


mpicc -g -c hello.c
mpicc -g -fPIC  -c  arltoolapi.c -lxml2 -I/usr/include/libxml2 


mpicc -g -shared -fPIC -o libarltool.so arltoolapi.o 


mpicc -g -o hello  hello.o libarltool.so
