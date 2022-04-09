package messaging.core.utils;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

public class LibraryLoader {

    private static final String PROPERTY_NAME = "lib.messaging.core.path";
    private static final String SUBSCRIBER_LIB = "libsubscriber.so";
    private static final String PUBLISHER_LIB = "libpublisher.so";

    private static FileInputStream fis;
    private static Properties property;

    static {
        property = new Properties();
        try {
            fis = new FileInputStream("src/main/resources/config.properties");
            property.load(fis);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public static String getLibPath(String libName){
        StringBuilder path = new StringBuilder();

        path.append(property.getProperty(PROPERTY_NAME));
        path.append(libName);

        return path.toString();
    }

    public static String getSubscriberLibPath(){

        return getLibPath(SUBSCRIBER_LIB);

    }

    public static String getPublisherLibPath(){

        return getLibPath(PUBLISHER_LIB);

    }

}
