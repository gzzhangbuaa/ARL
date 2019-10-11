#!/bin/bash
rm *.o *.a
mpicc  -g  -I../include/ -lxml2 -I/usr/include/libxml2   -c arltoolapi.c 
mpicc  -g  -I. -c hello.c 
mpicc  -g  -I. -c mywrapper.c
ar -rc  arltoolapi.a   arltoolapi.o  mywrapper.o
mpicc    hello.o -lxml2 -I/usr/include/libxml2  arltoolapi.a -o hello



