package messaging.core.persistence;

public class NullDbPersistentStorage extends PersistentStorage{
    public NullDbPersistentStorage() {
        super(DatabaseType.NULL_DB, null);
    }

    @Override
    public String serialize() {
        return "{" +
                "\"databaseType\": \"NULL_DB\", " +
                "\"dbName\": \"" + getDbName() + "\"" +
                "}";
    }
}
