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
public class Envelope {

    private String topic;
    private String uuid;
    private long sequenceNumber;
    private long timestamp;
    private boolean success;
    private String error;
    private String body;

    public static Envelope fromJson(String json) throws JsonProcessingException{
        Envelope envelope = new Envelope();
        envelope.deserialize(json);
        return envelope;
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

        Envelope msgEnvelope = mapper.readValue(json, this.getClass());

        uuid = msgEnvelope.getUuid();
        topic = msgEnvelope.getTopic();
        sequenceNumber = msgEnvelope.getSequenceNumber();
        timestamp = msgEnvelope.getTimestamp();
        success = msgEnvelope.isSuccess();
        error = msgEnvelope.getError();
        body = msgEnvelope.getBody();

    }



}
