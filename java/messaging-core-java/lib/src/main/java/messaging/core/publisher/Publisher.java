package messaging.core.publisher;


import messaging.core.messagetemplates.Message;

public class Publisher {

    static {
        //System.load("/home/andrey/Development/Projects/Ammer/messaging-core-java/lib/src/main/java/messaging/core/publisher/libpublisher.so");
        System.load("/home/andrey/Development/Projects/Ammer/messaging-core/core/java/libpublisher.so");
    }

    private long nativeObjectPointer;

    private native long nativeNew(String pubConnectionAddress, String repConnectionAddress);

    public Publisher(String pubConnectionAddress, String repConnectionAddress) {
        nativeObjectPointer = nativeNew(pubConnectionAddress, repConnectionAddress);
    }

    public Publisher(long pointer) {
        nativeObjectPointer = pointer;
    }

    public native void publish(long nativeObjectPointer, long sequenceNumber, String uuid, String topic, long timestamp, String messageType, boolean needsReply, String dataJson);

    public long getNativeObjectPointer(){
        return nativeObjectPointer;
    }

    public void publish(Message message){

        boolean needsReply = message.getNeeds_reply() == 1;

        publish(
                nativeObjectPointer,
                message.getSequence_number(),
                message.getUuid(),
                message.getTopic(),
                message.getTimestamp(),
                message.getMessage_type().name(),
                needsReply,
                message.getData_json()
        );
    }

}
