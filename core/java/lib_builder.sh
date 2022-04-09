#!/bin/bash


CORE_PATH="../src"

g++ -c -fPIC *.cpp utils/*.c $CORE_PATH/*.cpp $CORE_PATH/tools/*.c $CORE_PATH/tools/*.cpp -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/linux/ -lzmq -lpthread

echo =======================================================================
echo SUBSCRIBER LIBRARY BUILDING...
echo =======================================================================
g++ -fPIC *.o -shared -o libsubscriber.so -lzmq -lpthread
echo =======================================================================
echo SUBSCRIBER LIBRARY BUILDED!
echo =======================================================================

echo =======================================================================
echo PUBLISHER LIBRARY BUILDING...
echo =======================================================================
g++ -fPIC *.o -shared -o libpublisher.so -lzmq -lpthread
echo =======================================================================
echo PUBLISHER LIBRARY BUILDED!
echo =======================================================================
