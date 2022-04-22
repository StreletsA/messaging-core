package messaging.core.messagetemplates;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.*;


@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@ToString
@EqualsAndHashCode
public class Message {

    private String topic;
    private String uuid;
    private long sequenceNumber;
    private long timestamp;
    private String body;

    public static Message fromJson(String json) throws JsonProcessingException{
        Message message = new Message();
        message.deserialize(json);
        return message;
    }

    public String toJson() throws JsonProcessingException{
        return serialize();
    }

    private String serialize() throws JsonProcessingException {
        ObjectMapper mapper = new ObjectMapper();
        return mapper.writeValueAsString(this);
    }

    private void deserialize(String json) throws JsonProcessingException{
        ObjectMapper mapper = new ObjectMapper();

        Message msg = mapper.readValue(json, this.getClass());

        uuid = msg.getUuid();
        topic = msg.getTopic();
        sequenceNumber = msg.getSequenceNumber();
        timestamp = msg.getTimestamp();
        body = msg.getBody();

    }



}
