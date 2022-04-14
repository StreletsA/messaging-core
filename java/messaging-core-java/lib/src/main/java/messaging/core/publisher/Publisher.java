package messaging.core.publisher;

import messaging.core.messagetemplates.Message;
import messaging.core.messagetemplates.MessageType;
import messaging.core.persistence.PersistentStorage;

public class Publisher {

    private NativePublisher publisher;
    private long nativeObjectPointer;
    private final String pubConnectionAddress;
    private final String repConnectionAddress;

    public Publisher(PersistentStorage storage, String pubConnectionAddress, String repConnectionAddress){

        this.pubConnectionAddress = pubConnectionAddress;
        this.repConnectionAddress = repConnectionAddress;

        Thread pubCreator = new Thread(() -> {
            publisher = new NativePublisher(storage.serialize(), pubConnectionAddress, repConnectionAddress);
            nativeObjectPointer = publisher.getNativeObjectPointer();
        });

        pubCreator.start();

//        try {
//            pubCreator.join();
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }
    }

    public String getPubConnectionAddress() {
        return pubConnectionAddress;
    }
    public String getRepConnectionAddress() {
        return repConnectionAddress;
    }

    public void publish(long sequenceNumber, String uuid, String topic, long timestamp, MessageType messageType, boolean needsReply, String dataJson){
        publisher.publish(
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
        publisher.publishByJson(nativeObjectPointer, jsonMessage);
    }

}
