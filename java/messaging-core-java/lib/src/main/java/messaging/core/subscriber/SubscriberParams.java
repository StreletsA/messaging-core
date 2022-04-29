package messaging.core.subscriber;

import lombok.*;
import messaging.core.persistence.PersistentStorage;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
public class SubscriberParams {

    private PersistentStorage storage;
    private String topic;
    private String adapterName;
    private String subConnectionAddress;
    private String reqConnectionAddress;

}
