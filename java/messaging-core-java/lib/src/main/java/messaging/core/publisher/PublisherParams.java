package messaging.core.publisher;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import messaging.core.persistence.PersistentStorage;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
public class PublisherParams {

    private PersistentStorage storage;
    private String pubConnectionAddress;
    private String repConnectionAddress;

}
