package messaging.core.config;

public class Configuration {

    private static String LIB_MESSAGING_CORE_PATH;

    public static void setLibMessagingCorePath(String libMessagingCorePath){
        LIB_MESSAGING_CORE_PATH = libMessagingCorePath;
    }

    public static String getLibMessagingCorePath(){

        if (LIB_MESSAGING_CORE_PATH != null) {
            return LIB_MESSAGING_CORE_PATH;
        }
        else{
            throw new NullPointerException("You need to set Messaging Core path in configuration!");
        }

    }

}
