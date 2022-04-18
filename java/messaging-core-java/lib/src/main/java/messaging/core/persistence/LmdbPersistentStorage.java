package messaging.core.persistence;

public class LmdbPersistentStorage extends PersistentStorage{

    public LmdbPersistentStorage(String dbName) {
        super(DatabaseType.LMDB, dbName);
    }

    @Override
    public String serialize() {
        return "{" +
                "\"databaseType\": \"LMDB\", " +
                "\"dbName\": \"" + getDbName() + "\" " +
                "}";
    }
}
