package messaging.core.persistence;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
@AllArgsConstructor
public abstract class PersistentStorage {

    private DatabaseType databaseType;
    private final String dbName = "messaging_core";

    public abstract String serialize();

}
