g++ -c -fPIC *.cpp ../../messaging-core/*.cpp ../../messaging-core/tools/*.c ../../messaging-core/tools/*.cpp -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/linux/ -lzmq -lpthread

g++ -fPIC *.o -shared -o libsubscriber.so -lzmq -lpthread
