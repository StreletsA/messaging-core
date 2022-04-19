package messaging.core;

import messaging.core.config.Configuration;
import messaging.core.messagetemplates.Message;
import messaging.core.persistence.NullDbPersistentStorage;
import messaging.core.persistence.PersistentStorage;
import messaging.core.persistence.PostgreSqlPersistentStorage;
import messaging.core.publisher.Publisher;
import messaging.core.subscriber.Subscriber;

import java.util.Optional;
import java.util.UUID;

public class Main {

    public static void main(String[] args) {

        Configuration.setLibMessagingCorePath("/home/andrey/Development/Projects/Ammer/messaging-core/core/java/");

        PersistentStorage storage = new PostgreSqlPersistentStorage("postgres", "postgres", "127.0.0.1", "5432", "messaging_core", "messages");
        Publisher publisher = new Publisher(storage, "tcp://*:4533", "tcp://*:9928");

        PersistentStorage subStorage = new NullDbPersistentStorage();
        Subscriber subscriber = new Subscriber(subStorage, "TRANSACTION", "tcp://localhost:4533", "tcp://localhost:9928");

        Sender sender = new Sender(publisher);
        sender.start();

        Poller poller = new Poller(subscriber);
        poller.start();

    }

}

// Just for test
class Sender extends Thread {

    private Publisher publisher;

    public Sender(Publisher publisher){
        this.publisher = publisher;
    }

    @Override
    public void run() {
        System.out.println("Sender is starting...");

        int seqNum = 0;
        while (seqNum < 20){

            try {
                Message message = new Message(
                        "TRANSACTION",
                        UUID.randomUUID().toString(),
                        seqNum,
                        23422,
                        "{\n\t\"string_data\":\t\"Good job!!!\"\n}"
                );

                String json = message.serialize();

                publisher.publish(json);

                //System.out.println("JAVA: SENDER: PUBLISHED JSON -> " + json);
            }catch (Exception e){
                e.printStackTrace();
            }
            seqNum++;
        }
    }
}

// Just for test
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
