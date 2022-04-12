package messaging.core.persistence;

public class PostgreSqlPersistentStorage extends PersistentStorage{

    public PostgreSqlPersistentStorage(String dbName, String user, String password, String hostAddress, String port) {
        super(dbName, user, password, hostAddress, port);
    }

}
