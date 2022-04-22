package messaging.core.subscriber;

import lombok.extern.slf4j.Slf4j;
import messaging.core.messagetemplates.Envelope;
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

        Optional<String> strEnvelopeOptional = Optional.empty();

        try{

            String strEnvelope = subscriber.poll(nativeObjectPointer);

            if (strEnvelope.length() > 0) {
                strEnvelopeOptional = Optional.of(strEnvelope);
            }

        }catch (Exception e){
            log.error(e.getMessage());
        }

        return strEnvelopeOptional;
    }

    public Optional<Envelope> pollMessageEnvelope(){

        Optional<Envelope> messageEnvelopeOptional = Optional.empty();
        Optional<String> strMessageEnvelope = poll();

        if (strMessageEnvelope.isPresent()){

            String json = strMessageEnvelope.get();
            messageEnvelopeOptional = getMessageFromJson(json);

        }

        return messageEnvelopeOptional;
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

    private Optional<Envelope> getMessageFromJson(String json){

        Optional<Envelope> envelopeOptional = Optional.empty();

        try {
            Envelope envelope = Envelope.fromJson(json);
            envelopeOptional = Optional.of(envelope);
        }catch (Exception e){
            log.error(e.getMessage());
        }

        return envelopeOptional;

    }


}
