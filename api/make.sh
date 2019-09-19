#!/bin/bash


##mpicc -o hello hello.c arltoolapi.c -lxml2 -I/usr/include/libxml2


rm *.o *.a

mpicc  -g  -I../include/ -lxml2 -I/usr/include/libxml2   -c arltoolapi.c 

mpicc  -g  -I. -c hello.c 

ar -rc  arltoolapi.a   arltoolapi.o 


#mpicc   -o hello    hello.o  arltoolapi.a -lxml2 -I/usr/include/libxml2
mpicc    hello.o -lxml2 -I/usr/include/libxml2  arltoolapi.a -o hello



