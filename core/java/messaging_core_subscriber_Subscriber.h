/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class messaging_core_subscriber_Subscriber */

#ifndef _Included_messaging_core_subscriber_Subscriber
#define _Included_messaging_core_subscriber_Subscriber
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     messaging_core_subscriber_Subscriber
 * Method:    nativeNew
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_messaging_core_subscriber_Subscriber_nativeNew
  (JNIEnv *, jobject, jstring, jstring, jstring);

/*
 * Class:     messaging_core_subscriber_Subscriber
 * Method:    join
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_messaging_core_subscriber_Subscriber_join
  (JNIEnv *, jobject, jlong);

/*
 * Class:     messaging_core_subscriber_Subscriber
 * Method:    poll
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_messaging_core_subscriber_Subscriber_poll
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
