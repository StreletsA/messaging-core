package messaging.core.subscriber;

/*
javac -h . Subscriber.java
 */

import messaging.core.utils.LibraryLoader;

public class NativeSubscriber {


    private long nativeObjectPointer;

    static {
        LibraryLoader.loadSubscriberLib();
    }

    public NativeSubscriber(String topic, String subConnectionAddress, String reqConnectionAddress) {
        nativeObjectPointer = nativeNew(topic, subConnectionAddress, reqConnectionAddress);
    }

    private native long nativeNew(String topic, String subConnectionAddress, String reqConnectionAddress);

    // return message as json
    public native String poll(long nativeObjectPointer);

    public long getNativeObjectPointer(){
        return nativeObjectPointer;
    }
}
