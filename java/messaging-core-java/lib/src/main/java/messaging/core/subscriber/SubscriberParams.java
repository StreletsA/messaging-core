package messaging.core.subscriber;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import messaging.core.persistence.PersistentStorage;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
public class SubscriberParams {

    private PersistentStorage storage;
    private String topic;
    private String subConnectionAddress;
    private String reqConnectionAddress;

}
