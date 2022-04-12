package messaging.core;

import messaging.core.messagetemplates.Message;
import messaging.core.messagetemplates.MessageType;
import messaging.core.persistence.PersistentStorage;
import messaging.core.persistence.PostgreSqlPersistentStorage;
import messaging.core.publisher.Publisher;
import messaging.core.subscriber.Subscriber;

import java.util.Optional;

public class Main {

    public static void main(String[] args) throws InterruptedException {

        PersistentStorage storage = new PostgreSqlPersistentStorage("postgres", "postgres", "127.0.0.1", "5432");

        Publisher publisher = new Publisher(storage,"tcp://*:2033", "tcp://*:5553");
        Subscriber subscriber = new Subscriber("TEST", "tcp://localhost:2033", "tcp://localhost:5553");

        Sender sender = new Sender(publisher);
        sender.start();
        Poller poller = new Poller(subscriber);
        poller.start();

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

        int seqNum = 1;
        while (seqNum < 20){

            try {
                Message message = new Message(
                        "uuid",
                        "TEST",
                        seqNum,
                        23422,
                        false,
                        MessageType.INFO,
                        "{\n\t\"string_data\":\t\"Good job!!!\"\n}"
                );

                String json = message.serialize();

                publisher.publish(json);
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

        Optional<Message> messageOptional = Optional.empty();

        while (true){

            messageOptional = subscriber.pollMessage();
            messageOptional.ifPresent(System.out::println);

        }
    }

}
