package messaging.core.utils;

import messaging.core.config.Configuration;

public class LibraryLoader {

    // Library's name is 'libmessaging_core.so. 'lib' just a prefix for linux library loader system'
    private static final String MESSAGING_CORE_LIB = "libmessaging_core.so";

    public static String getLibPath(String libName){
        StringBuilder path = new StringBuilder();

        path.append(Configuration.getLibMessagingCorePath());
        path.append(libName);

        return path.toString();
    }

    public static String getMessagingCoreLibPath(){

        return getLibPath(MESSAGING_CORE_LIB);

    }

    public static void loadMessagingCoreLib(){

        System.load(getMessagingCoreLibPath());

    }

}
