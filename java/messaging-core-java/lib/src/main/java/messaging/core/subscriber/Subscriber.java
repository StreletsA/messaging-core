package messaging.core.subscriber;

import lombok.extern.slf4j.Slf4j;
import messaging.core.messagetemplates.Message;
import messaging.core.persistence.PersistentStorage;

import java.util.Optional;

@Slf4j
public class Subscriber {

    private final String topic;
    private final String subConnectionAddress;
    private final String reqConnectionAddress;

    private NativeSubscriber subscriber;
    private long nativeObjectPointer;

    public Subscriber(PersistentStorage storage, String topic, String subConnectionAddress, String reqConnectionAddress) {

        this.topic = topic;
        this.subConnectionAddress = subConnectionAddress;
        this.reqConnectionAddress = reqConnectionAddress;

        log.info("Subscriber creating...");
        Thread subCreator = new Thread(() -> {

            subscriber = new NativeSubscriber(storage.serialize(), topic, subConnectionAddress, reqConnectionAddress);
            nativeObjectPointer = subscriber.getNativeObjectPointer();

        });

        subCreator.start();

        try {
            subCreator.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        log.info("Subscriber created!");

    }

    public String poll(){
        String ans = "";

        try{
            ans = subscriber.poll(nativeObjectPointer);
        }catch (NullPointerException e){
            log.error("Subscriber is null!");
        }

        return ans;
    }
    public Optional<Message> pollMessage(){

        Optional<Message> messageOptional = Optional.empty();

        try{
            String json = poll();
            if (json.length() > 0) {
                messageOptional = Optional.of(new Message(json));
            }
        }catch (IllegalStateException e){
            log.error("Json is illegal!");
            e.printStackTrace();
            messageOptional = Optional.empty();
        }

        return messageOptional;
    }

}
