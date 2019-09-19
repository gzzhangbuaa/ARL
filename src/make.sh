#!/bin/bash
rm *.o

gcc -fPIC -g -I../include/  -lxml2 -I/usr/include/libxml2 -c findInXml.c -o findInXml.o
gcc -fPIC -g -I../include/ -c tools.c -o tools.o -lxml2 -I/usr/include/libxml2
gcc -fPIC -g -I../include/ -c Monitor.c -o Monitor.o -lxml2 -I/usr/include/libxml2
gcc -fPIC -g -I../include/ -c genshcommand.c -o genshcommand.o -lxml2 -I/usr/include/libxml2
gcc -fPIC -g -I../include/ -c settimer.c -o settimer.o -lxml2 -I/usr/include/libxml2
gcc -fPIC -g -I../include/ -c checkstate.c -o checkstate.o -lxml2 -I/usr/include/libxml2
gcc -fPIC -g -I../include/ -c submit.c -o submit.o -lxml2 -I/usr/include/libxml2
gcc -fPIC -g -I../include/ -c tcpcomm.c -o tcpcomm.o -lxml2 -I/usr/include/libxml2 
 
gcc findInXml.o tools.o Monitor.o genshcommand.o settimer.o checkstate.o submit.o tcpcomm.o -lxml2 -I/usr/include/libxml2  -o ARLTool -lpthread
 
##gcc -fPIC -g -I../include/ -lxml2 -I/usr/include/libxml2  genshcommand.c -o genshcommand

#gcc Monitor.c tools.c findInXml.c -o Monitor -I../include/ -I/usr/include/libxml2 -lxml2


gcc -fPIC -g  -lxml2 -I/usr/include/libxml2 -I../include configure-gen.c findInXml.c -o configure-gen 

##gcc  -o  -lxml2 -I/usr/include/libxml2
