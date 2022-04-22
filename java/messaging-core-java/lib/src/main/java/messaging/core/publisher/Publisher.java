package messaging.core.publisher;

import lombok.extern.slf4j.Slf4j;
import messaging.core.messagetemplates.Message;
import messaging.core.persistence.PersistentStorage;

@Slf4j
public class Publisher {

    private final PublisherParams publisherParams;

    private Thread publisherCreator;
    private NativePublisher nativePublisher;
    private long nativeObjectPointer;

    public Publisher(PublisherParams publisherParams){

        this.publisherParams = publisherParams;

        log.info("Publisher creating...");
        createNativePublisher();
        log.info("Publisher created!");

    }

    public PublisherParams getPublisherParams(){
        return publisherParams;
    }

    public boolean isPublisherReady(){
        return nativePublisher != null;
    }

    public void publish(Message message) {

        String topic = message.getTopic();
        String uuid = message.getUuid();
        long sequenceNumber = message.getSequenceNumber();
        long timestamp = message.getTimestamp();
        String body = message.getBody();

        nativePublisher.publish(
                nativeObjectPointer,
                topic,
                uuid,
                sequenceNumber,
                timestamp,
                body
        );
    }

    public void publishByJson(String jsonMessage){
        nativePublisher.publishByJson(nativeObjectPointer, jsonMessage);
    }

    private void createNativePublisher(){

        PersistentStorage storage = publisherParams.getStorage();
        String pubConnectionAddress = publisherParams.getPubConnectionAddress();
        String repConnectionAddress = publisherParams.getRepConnectionAddress();

        publisherCreator = new Thread(() -> {
            nativePublisher = new NativePublisher(storage.serialize(), pubConnectionAddress, repConnectionAddress);
            nativeObjectPointer = nativePublisher.getNativeObjectPointer();
        });

        publisherCreator.start();
        //tryJoin();

    }

    private void tryJoin(){

        try {
            publisherCreator.join();
        } catch (InterruptedException e) {
            log.error(e.getMessage());
        }

    }


}
