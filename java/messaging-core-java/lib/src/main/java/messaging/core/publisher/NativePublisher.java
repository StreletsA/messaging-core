package messaging.core.publisher;

/*
javac -h . PublisherNative.java
 */

import messaging.core.utils.LibraryLoader;

public class NativePublisher {

    private long nativeObjectPointer;


    static {
        LibraryLoader.loadPublisherLib();
    }

    public NativePublisher(String persistentStorageJsonParams, String pubConnectionAddress, String repConnectionAddress) {
        nativeObjectPointer = nativeNew(persistentStorageJsonParams, pubConnectionAddress, repConnectionAddress);
    }

    private native long nativeNew(String persistentStorageJsonParams, String pubConnectionAddress, String repConnectionAddress);

    public long getNativeObjectPointer() {
        return nativeObjectPointer;
    }

    public native void publish(long nativeObjectPointer, long sequenceNumber, String uuid, String topic, long timestamp, String messageType, boolean needsReply, String dataJson);
    public native void publishByJson(long nativeObjectPointer, String jsonMessage);

}
