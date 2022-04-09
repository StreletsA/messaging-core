package messaging.core.messagetemplates;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
public class Message {

    private String uuid;
    private String topic;
    private long sequence_number;
    private long timestamp;
    private boolean needs_reply;
    private MessageType message_type;
    private String data_json;



    public String serialize(){
        ObjectMapper mapper = new ObjectMapper();
        try {
            return mapper.writeValueAsString(this);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }
        return null;
    }

    public boolean deserialize(String json){
        return true;
    }



}
