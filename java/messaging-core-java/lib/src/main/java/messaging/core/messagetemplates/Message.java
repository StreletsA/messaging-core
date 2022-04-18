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
    private long sequence_number;
    private long timestamp;
    private String body;

    public Message(String json) throws IllegalStateException{
        if (!deserialize(json)){
            throw new IllegalStateException("JSON is incorrect!");
        }
    }

    public String serialize(){
        ObjectMapper mapper = new ObjectMapper();
        try {
            return mapper.writeValueAsString(this);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }
        return null;
    }

    public boolean deserialize(String json) {
        ObjectMapper mapper = new ObjectMapper();

        try {

            Message msg = mapper.readValue(json, this.getClass());

            uuid = msg.getUuid();
            topic = msg.getTopic();
            sequence_number = msg.getSequence_number();
            timestamp = msg.getTimestamp();
            body = msg.getBody();

        } catch (JsonProcessingException e) {
            e.printStackTrace();
            return false;
        }

        return true;

    }



}
