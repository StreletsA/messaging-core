package messaging.core;

import messaging.core.config.Configuration;
import messaging.core.messagetemplates.Envelope;
import messaging.core.persistence.NullDbPersistentStorage;
import messaging.core.persistence.PersistentStorage;
import messaging.core.persistence.PostgreSqlPersistentStorage;
import messaging.core.publisher.Publisher;
import messaging.core.publisher.PublisherParams;
import messaging.core.subscriber.Subscriber;
import messaging.core.subscriber.SubscriberParams;

import java.util.Optional;
import java.util.UUID;

public class Main {

    public static void main(String[] args) {

        Configuration.setLibMessagingCorePath("/home/andrey/Development/Projects/Ammer/messaging-core/core/java/");

        PersistentStorage pubStorage = new PostgreSqlPersistentStorage("postgres", "postgres", "127.0.0.1", "5432", "messaging_core", "pub");
        PersistentStorage subStorage = new NullDbPersistentStorage();

        SubscriberParams subscriberParams = new SubscriberParams(subStorage, "TRANSACTION", "eth", "tcp://localhost:4533", "tcp://localhost:9928");
        PublisherParams publisherParams = new PublisherParams(pubStorage, "tcp://*:4533", "tcp://*:9928");

        Publisher publisher = new Publisher(publisherParams);
        Subscriber subscriber = new Subscriber(subscriberParams);

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
                Envelope message = new Envelope(
                        "TRANSACTION",
                        UUID.randomUUID().toString(),
                        seqNum,
                        23422,
                        true,
                        "Empty",
                        "{\n\t\"string_data\":\t\"Good job!!!\"\n}"
                );

                String json = message.toJson();
                publisher.publishByJson(json);
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

        Optional<Envelope> envelopeOptional = Optional.empty();

        int i = 0;
        while (true){
            i++;

            envelopeOptional = subscriber.pollMessageEnvelope();
            envelopeOptional.ifPresent(envelope -> System.out.println("JAVA: POLLER -> " + envelope));

        }
    }


}
