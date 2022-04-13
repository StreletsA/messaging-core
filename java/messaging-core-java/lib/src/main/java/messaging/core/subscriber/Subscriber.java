package messaging.core.subscriber;

/*
javac -h . Subscriber.java
 */

import messaging.core.messagetemplates.Message;
import messaging.core.utils.LibraryLoader;

import java.util.Optional;

public class Subscriber {


    private long nativeObjectPointer;
    private final String topic;
    private final String subConnectionAddress;
    private final String reqConnectionAddress;

    static {
        LibraryLoader.loadSubscriberLib();
    }

    public Subscriber(String topic, String subConnectionAddress, String reqConnectionAddress) {
        this.topic = topic;
        this.subConnectionAddress = subConnectionAddress;
        this.reqConnectionAddress = reqConnectionAddress;

        nativeObjectPointer = nativeNew(topic, subConnectionAddress, reqConnectionAddress);
    }

    private native long nativeNew(String topic, String subConnectionAddress, String reqConnectionAddress);

    private native void join(long nativeObjectPointer);
    private native void stop(long nativeObjectPointer);

    // return message as json
    public native String poll(long nativeObjectPointer);
    public String poll(){
        return poll(nativeObjectPointer);
    }
    public Optional<Message> pollMessage(){

        Optional<Message> messageOptional = Optional.empty();

        try{
            String json = poll();
            System.out.println("JAVA: SUBSCRIBER: POLLED JSON -> " + json);
            if (json.length() > 0) {
                messageOptional = Optional.of(new Message(json));
            }
        }catch (IllegalStateException e){
            e.printStackTrace();
            messageOptional = Optional.empty();
        }

        return messageOptional;
    }

    public long getNativeObjectPointer(){
        return nativeObjectPointer;
    }
    public String getTopic(){
        return topic;
    }
    public String getSubConnectionAddress(){
        return subConnectionAddress;
    }
    public String getReqConnectionAddress(){
        return reqConnectionAddress;
    }
}
