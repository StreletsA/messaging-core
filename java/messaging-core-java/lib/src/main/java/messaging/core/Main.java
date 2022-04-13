package messaging.core;

import messaging.core.messagetemplates.Message;
import messaging.core.messagetemplates.MessageType;
import messaging.core.persistence.PersistentStorage;
import messaging.core.persistence.PostgreSqlPersistentStorage;
import messaging.core.publisher.Publisher;
import messaging.core.subscriber.Subscriber;

import java.util.Optional;
import java.util.UUID;

public class Main {

    static Publisher publisher;
    static Subscriber subscriber;

    public static void main(String[] args) throws InterruptedException {

        Thread pub = new Thread(() -> {
            PersistentStorage storage = new PostgreSqlPersistentStorage("postgres", "postgres", "127.0.0.1", "5432");
            publisher = new Publisher(storage,"tcp://*:2033", "tcp://*:5553");
        });
        Thread sub = new Thread(() -> {
            subscriber = new Subscriber("TEST", "tcp://localhost:2033", "tcp://localhost:5553");
        });

        pub.start();
        pub.join();

        sub.start();
        sub.join();

        Sender sender = new Sender(publisher);
        sender.start();
        sender.join();
        
        Poller poller = new Poller(subscriber);
        poller.start();
        poller.join();

    }

}

class Sender extends Thread {

    private Publisher publisher;

    public Sender(Publisher publisher){
        this.publisher = publisher;
    }

    @Override
    public void run() {
        System.out.println("Sender is starting...");

        int seqNum = 0;
        while (true){

            try {
                Message message = new Message(
                        UUID.randomUUID().toString(),
                        "TEST",
                        seqNum,
                        23422,
                        false,
                        MessageType.INFO,
                        "{\n\t\"string_data\":\t\"Good job!!!\"\n}"
                );

                String json = message.serialize();

                publisher.publish(json);

                System.out.println("JAVA: SENDER: PUBLISHED JSON -> " + json);
            }catch (Exception e){
                e.printStackTrace();
            }
            seqNum++;
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

        Optional<Message> messageOptional = Optional.empty();

        int i = 0;
        while (true){
            i++;

            messageOptional = subscriber.pollMessage();
            messageOptional.ifPresent(msg -> System.out.println("JAVA: POLLER -> " + msg));

        }
    }


}
