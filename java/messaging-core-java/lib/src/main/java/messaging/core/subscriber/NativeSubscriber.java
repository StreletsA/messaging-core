package messaging.core.subscriber;

/*
javac -h . Subscriber.java
 */

import messaging.core.utils.LibraryLoader;

public class NativeSubscriber {


    private long nativeObjectPointer;

    static {
        LibraryLoader.loadMessagingCoreLib();
    }

    public NativeSubscriber(String persistentStorageJsonParams, String topic, String subConnectionAddress, String reqConnectionAddress) {
        nativeObjectPointer = nativeNew(persistentStorageJsonParams, topic, subConnectionAddress, reqConnectionAddress);
    }

    private native long nativeNew(String persistentStorageJsonParams, String topic, String subConnectionAddress, String reqConnectionAddress);

    // return message as json
    public native String poll(long nativeObjectPointer);

    public long getNativeObjectPointer(){
        return nativeObjectPointer;
    }
}
