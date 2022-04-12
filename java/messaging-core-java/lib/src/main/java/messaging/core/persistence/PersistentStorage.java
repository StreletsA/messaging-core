package messaging.core.persistence;

import lombok.AllArgsConstructor;
import lombok.Getter;

@Getter
@AllArgsConstructor
public abstract class PersistentStorage {

    private String dbName;
    private String user;
    private String password;
    private String hostAddress;
    private String port;

}
