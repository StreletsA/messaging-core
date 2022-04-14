package messaging.core.messagetemplates;

import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
public class MessageTest {

    private static String legalMessageJson = "{\"uuid\":\"test_uuid\",\"topic\":\"TEST\",\"sequence_number\":8734813814637,\"timestamp\":3456784324,\"needs_reply\":false,\"message_type\":\"SUCCESS\",\"data_json\":\"{\"string_data\": \"Test string data\"}\"}";

    @Disabled
    @Test
    void legalJsonDeserialize(){

        Message message = new Message(
                "test_uuid",
                "TEST",
                8734813814637L,
                3456784324L,
                false,
                MessageType.SUCCESS,
                "{\"string_data\": \"Test string data\"}"
        );

        Message deserializedMessage = new Message(legalMessageJson);

        assertEquals(message, deserializedMessage);

    }

    @Test
    void serializeAndDeserialize(){

        Message message = new Message(
                "test_uuid",
                "TEST",
                8734813814637L,
                3456784324L,
                false,
                MessageType.SUCCESS,
                "{\"string_data\": \"Test string data\"}"
        );

        Message deserializedMessage = new Message(message.serialize());

        assertEquals(message, deserializedMessage);

    }

}
