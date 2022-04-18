package messaging.core.persistence;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
@AllArgsConstructor
public abstract class PersistentStorage {

    private DatabaseType databaseType;
    private String dbName;

    public abstract String serialize();

}
