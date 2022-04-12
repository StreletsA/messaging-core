package messaging.core.publisher;

/*
javac -h . Subscriber.java
 */

import messaging.core.messagetemplates.Message;
import messaging.core.messagetemplates.MessageType;
import messaging.core.persistence.PersistentStorage;
import messaging.core.utils.LibraryLoader;

public class Publisher {

    private long nativeObjectPointer;
    private final String pubConnectionAddress;
    private final String repConnectionAddress;

    static {
        LibraryLoader.loadPublisherLib();
    }

    public Publisher(PersistentStorage storage, String pubConnectionAddress, String repConnectionAddress) {
        this.pubConnectionAddress = pubConnectionAddress;
        this.repConnectionAddress = repConnectionAddress;
        nativeObjectPointer = nativeNew(storage.serialize(), pubConnectionAddress, repConnectionAddress);
    }

    private native long nativeNew(String persistentStorageJsonParams, String pubConnectionAddress, String repConnectionAddress);

    public long getNativeObjectPointer() {
        return nativeObjectPointer;
    }
    public String getPubConnectionAddress() {
        return pubConnectionAddress;
    }
    public String getRepConnectionAddress() {
        return repConnectionAddress;
    }

    public native void publish(long nativeObjectPointer, long sequenceNumber, String uuid, String topic, long timestamp, String messageType, boolean needsReply, String dataJson);
    public native void publishByJson(long nativeObjectPointer, String jsonMessage);

    public void publish(long sequenceNumber, String uuid, String topic, long timestamp, MessageType messageType, boolean needsReply, String dataJson){
        publish(
                nativeObjectPointer,
                sequenceNumber,
                uuid,
                topic,
                timestamp,
                messageType.name(),
                needsReply,
                dataJson
        );
    }
    public void publish(Message message) {
        publish(
                message.getSequence_number(),
                message.getUuid(),
                message.getTopic(),
                message.getTimestamp(),
                message.getMessage_type(),
                message.isNeeds_reply(),
                message.getData_json()
        );
    }
    public void publish(String jsonMessage){
        publishByJson(nativeObjectPointer, jsonMessage);
    }

}
