package messaging.core.publisher;

import lombok.extern.slf4j.Slf4j;
import messaging.core.messagetemplates.Message;
import messaging.core.persistence.PersistentStorage;

@Slf4j
public class Publisher {

    private NativePublisher publisher = null;
    private long nativeObjectPointer;
    private final String pubConnectionAddress;
    private final String repConnectionAddress;

    public Publisher(PersistentStorage storage, String pubConnectionAddress, String repConnectionAddress){

        this.pubConnectionAddress = pubConnectionAddress;
        this.repConnectionAddress = repConnectionAddress;

        log.info("Publisher creating...");
        Thread pubCreator = new Thread(() -> {
            publisher = new NativePublisher(storage.serialize(), pubConnectionAddress, repConnectionAddress);
            nativeObjectPointer = publisher.getNativeObjectPointer();
            log.info("Publisher created!");
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

    public void publish(String topic, String uuid, long sequenceNumber, long timestamp, String body){
        publisher.publish(
                nativeObjectPointer,
                topic,
                uuid,
                sequenceNumber,
                timestamp,
                body
        );
    }
    public void publish(Message message) {
        publish(
                message.getTopic(),
                message.getUuid(),
                message.getSequenceNumber(),
                message.getTimestamp(),
                message.getBody()
        );
    }
    public void publish(String jsonMessage){
        publisher.publishByJson(nativeObjectPointer, jsonMessage);
    }

    public boolean isPublisherReady(){
        return publisher != null;
    }

}
