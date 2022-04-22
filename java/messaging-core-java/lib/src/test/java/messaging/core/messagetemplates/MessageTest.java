package messaging.core.messagetemplates;

import com.fasterxml.jackson.core.JsonProcessingException;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
public class MessageTest {

    private static String legalMessageJson = "{\"uuid\":\"test_uuid\",\"topic\":\"TEST\",\"sequence_number\":8734813814637,\"timestamp\":3456784324,\"needs_reply\":false,\"message_type\":\"SUCCESS\",\"data_json\":\"{\"string_data\": \"Test string data\"}\"}";

    @Disabled
    @Test
    void legalJsonDeserialize() throws JsonProcessingException {

        Message message = new Message(
                "test_uuid",
                "TEST",
                8734813814637L,
                3456784324L,
                "{\"string_data\": \"Test string data\"}"
        );

        Message deserializedMessage = Message.fromJson(legalMessageJson);

        assertEquals(message, deserializedMessage);

    }

    @Disabled
    @Test
    void serializeAndDeserialize() throws JsonProcessingException {

        Message message = new Message(
                "test_uuid",
                "TEST",
                8734813814637L,
                3456784324L,
                "{\"string_data\": \"Test string data\"}"
        );

        Message deserializedMessage = Message.fromJson(message.toJson());

        assertEquals(message, deserializedMessage);

    }

}
