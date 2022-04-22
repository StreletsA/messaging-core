package messaging.core.subscriber;

import lombok.extern.slf4j.Slf4j;
import messaging.core.messagetemplates.Message;
import messaging.core.persistence.PersistentStorage;

import java.util.Optional;

@Slf4j
public class Subscriber{

    private final SubscriberParams subscriberParams;

    private Thread subscriberCreator;
    private NativeSubscriber subscriber;
    private long nativeObjectPointer;

    public Subscriber(SubscriberParams subscriberParams) {

        this.subscriberParams = subscriberParams;

        log.info("Subscriber creating...");
        createNativeSubscriber();
        log.info("Subscriber created!");

    }

    public boolean isSubscriberReady(){
        return subscriber != null;
    }

    public SubscriberParams getSubscriberParams(){
        return subscriberParams;
    }

    public Optional<String> poll(){

        Optional<String> strMessageOptional = Optional.empty();

        try{

            String strMessage = subscriber.poll(nativeObjectPointer);

            if (strMessage.length() > 0) {
                strMessageOptional = Optional.of(strMessage);
            }

        }catch (Exception e){
            log.error(e.getMessage());
        }

        return strMessageOptional;
    }

    public Optional<Message> pollMessage(){

        Optional<Message> messageOptional = Optional.empty();
        Optional<String> strMessage = poll();

        if (strMessage.isPresent()){

            String json = strMessage.get();
            messageOptional = getMessageFromJson(json);

        }

        return messageOptional;
    }

    private void createNativeSubscriber(){

        PersistentStorage storage = subscriberParams.getStorage();
        String topic = subscriberParams.getTopic();
        String subConnectionAddress = subscriberParams.getSubConnectionAddress();
        String reqConnectionAddress = subscriberParams.getReqConnectionAddress();

        subscriberCreator = new Thread(() -> {

            subscriber = new NativeSubscriber(storage.serialize(), topic, subConnectionAddress, reqConnectionAddress);
            nativeObjectPointer = subscriber.getNativeObjectPointer();

        });

        subscriberCreator.start();

        //tryJoin();

    }

    private void tryJoin(){

        try {
            subscriberCreator.join();
        } catch (InterruptedException e) {
            log.error(e.getMessage());
        }

    }

    private Optional<Message> getMessageFromJson(String json){

        Optional<Message> messageOptional = Optional.empty();

        try {
            Message message = Message.fromJson(json);
            messageOptional = Optional.of(message);
        }catch (Exception e){
            log.error(e.getMessage());
        }

        return messageOptional;

    }


}
