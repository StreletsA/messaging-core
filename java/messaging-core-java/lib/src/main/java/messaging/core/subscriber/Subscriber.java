package messaging.core.subscriber;

import messaging.core.messagetemplates.Message;

import java.util.Optional;

public class Subscriber {

    private final String topic;
    private final String subConnectionAddress;
    private final String reqConnectionAddress;

    private NativeSubscriber subscriber;
    private long nativeObjectPointer;

    public Subscriber(String topic, String subConnectionAddress, String reqConnectionAddress) {

        this.topic = topic;
        this.subConnectionAddress = subConnectionAddress;
        this.reqConnectionAddress = reqConnectionAddress;

        Thread subCreator = new Thread(() -> {

            subscriber = new NativeSubscriber(topic, subConnectionAddress, reqConnectionAddress);
            nativeObjectPointer = subscriber.getNativeObjectPointer();

        });

        subCreator.start();

        try {
            subCreator.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }

    public String poll(){
        return subscriber.poll(nativeObjectPointer);
    }
    public Optional<Message> pollMessage(){

        Optional<Message> messageOptional = Optional.empty();

        try{
            String json = poll();
            if (json.length() > 0) {
                messageOptional = Optional.of(new Message(json));
            }
        }catch (IllegalStateException e){
            e.printStackTrace();
            messageOptional = Optional.empty();
        }

        return messageOptional;
    }

}
