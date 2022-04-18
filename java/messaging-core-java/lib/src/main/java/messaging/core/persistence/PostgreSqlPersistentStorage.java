package messaging.core.persistence;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class PostgreSqlPersistentStorage extends PersistentStorage{

    private String user;
    private String password;
    private String hostAddress;
    private String port;

    public PostgreSqlPersistentStorage(String user, String password, String hostAddress, String port, String dbName) {
        super(DatabaseType.POSTGRESQL, dbName);
        this.user = user;
        this.password = password;
        this.hostAddress = hostAddress;
        this.port = port;

    }

    public PostgreSqlPersistentStorage(String jsonParams){
        super(DatabaseType.POSTGRESQL, "");

        ObjectMapper mapper = new ObjectMapper();

        try {

            PostgreSqlPersistentStorage storage = mapper.readValue(jsonParams, this.getClass());

            setDbName(storage.getDbName());
            user = storage.getUser();
            password = storage.getPassword();
            hostAddress = storage.getHostAddress();
            port = storage.getPort();

        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }

    }

    @Override
    public String serialize(){

        return "{" +
                "\"databaseType\": \"POSTGRESQL\", " +
                "\"dbName\": \"" + getDbName() + "\", " +
                "\"user\": \"" + user + "\", " +
                "\"password\": \"" + password + "\", " +
                "\"hostAddress\": \"" + hostAddress + "\", " +
                "\"port\": \"" + port + "\"" +
                "}";

    }
}
