package messaging.core;

import messaging.core.messagetemplates.Message;
import messaging.core.messagetemplates.MessageType;
import messaging.core.publisher.Publisher;
import messaging.core.subscriber.Subscriber;

public class Main {

    public static void main(String[] args) throws InterruptedException {

        Publisher publisher = new Publisher("tcp://*:2033", "tcp://*:5553");
        Subscriber subscriber = new Subscriber("TEST", "tcp://localhost:2033", "tcp://localhost:5553");

        Sender sender = new Sender(publisher.getNativeObjectPointer());
        sender.start();
        Poller poller = new Poller(subscriber);
        poller.start();

    }

}

class Sender extends Thread {

    private long publisherPointer;

    public Sender(long publisher){
        this.publisherPointer = publisher;
    }

    @Override
    public void run() {
        System.out.println("Sender is starting...");
        System.out.println("Pointer is " + publisherPointer);
        int seqNum = 1;
        while (true){



            try {
                Message message = new Message(
                        "uuid",
                        "TEST",
                        seqNum,
                        23422,
                        0,
                        MessageType.INFO,
                        "{\n\t\"string_data\":\t\"Good job!!!\"\n}"
                );

                String messageJson = """
                        {
                           "uuid":	"wewfeer",
                           "topic":	"TEST",
                           "sequence_number":	836,
                           "timestamp":	234241,
                           "needs_reply":	0,
                           "message_type":	"INFO",
                           "data_json":	"{\n\t\"string_data\":\t\"Good job!!!\"\n}"
                        }
                        """;

                Publisher publisherClass = new Publisher(publisherPointer);
                publisherClass.publish(message);
                //publisherClass.publish(publisherClass.getNativeObjectPointer(), message.serialize());
                //publisherClass.publish(publisherPointer, seqNum, "123", "TEST", 434234, "INFO", false, "{\"string_data\":	\"Good job!!!\"}");
                seqNum++;
            }catch (Exception e){

            }

        }
    }
}

class Poller extends Thread{

    private Subscriber subscriber;

    public Poller(Subscriber subscriber){
        this.subscriber = subscriber;
    }

    @Override
    public void run(){
        System.out.println("Poller is starting...");
        while (true){
            String msg_json = subscriber.poll(subscriber.getNativeObjectPointer());
            if (!msg_json.equals("")){
                System.out.println("JAVA: " + msg_json);
            }
        }
    }

}
