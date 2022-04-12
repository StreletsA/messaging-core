package messaging.core.persistence;

public class PostgreSqlPersistentStorage extends PersistentStorage{


    public PostgreSqlPersistentStorage(DatabaseType databaseType, String dbName, String user, String password, String hostAddress, String port) {
        super(databaseType, dbName, user, password, hostAddress, port);
    }
}
