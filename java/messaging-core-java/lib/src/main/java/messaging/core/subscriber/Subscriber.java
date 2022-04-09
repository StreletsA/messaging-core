package messaging.core.subscriber;

/*
g++ -fPIC -shared -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/linux/ messaging_core_subscriber_Subscriber.cpp -o libsubscriber.so -lzmq -lpthread

javac -h . Subscriber.java
g++ -c -fPIC *.cpp tools/*.c tools/*.cpp -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/ -I/usr/lib/jvm/java-1.13.0-openjdk-amd64/include/linux/ -lzmq -lpthread
g++ -fPIC *.o -shared -o libsubscriber.so -lzmq -lpthread
 */

//import messaging.core.utils.LibraryLoader;

public class Subscriber {

    private long nativeObjectPointer;
    private final String topic;

    static {
        System.load("/home/andrey/Development/Projects/Ammer/messaging-core/core/java/libsubscriber.so");
    }

    public Subscriber(String topic, String subConnectionAddress, String reqConnectionAddress) {
        this.topic = topic;
        nativeObjectPointer = nativeNew(topic, subConnectionAddress, reqConnectionAddress);
    }

    private native long nativeNew(String topic, String subConnectionAddress, String reqConnectionAddress);

    // return message as json
    public native String poll(long nativeObjectPointer);
    public String getTopic(){
        return topic;
    }

    public native void start(long nativeObjectPointer);
    public native void join(long nativeObjectPointer);
    public native void detach(long nativeObjectPointer);

    public long getNativeObjectPointer(){
        return nativeObjectPointer;
    }

}
