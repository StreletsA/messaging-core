package messaging.core.publisher;

/*
javac -h . PublisherNative.java
 */

import messaging.core.utils.LibraryLoader;

public class NativePublisher {

    private long nativeObjectPointer;


    static {
        LibraryLoader.loadMessagingCoreLib();
    }

    public NativePublisher(String persistentStorageJsonParams, String pubConnectionAddress, String repConnectionAddress) {
        nativeObjectPointer = nativeNew(persistentStorageJsonParams, pubConnectionAddress, repConnectionAddress);
    }

    private native long nativeNew(String persistentStorageJsonParams, String pubConnectionAddress, String repConnectionAddress);

    public long getNativeObjectPointer() {
        return nativeObjectPointer;
    }

    public native void publish(long nativeObjectPointer, String topic, String uuid, long sequenceNumber, long timestamp, boolean success, String error, String body);
    public native void publishByJson(long nativeObjectPointer, String jsonMessage);

}
