#!/bin/bash


CORE_PATH="../src"

g++ -c -fPIC *.cpp utils/*.c $CORE_PATH/*.cpp $CORE_PATH/tools/*.c $CORE_PATH/tools/*.cpp -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/linux/ -lzmq -lpthread -lpqxx

gcc -c -fPIC ../src/lmdb/*.c 

echo =======================================================================
echo MESSAGING_CORE LIBRARY BUILDING...
echo =======================================================================
g++ -fPIC *.o -shared -o messaging_core.so -lzmq -lpthread -lpqxx
echo =======================================================================
echo MESSAGING_CORE LIBRARY BUILDED!
echo =======================================================================
