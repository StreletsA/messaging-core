/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class messaging_core_subscriber_NativeSubscriber */

#ifndef _Included_messaging_core_subscriber_NativeSubscriber
#define _Included_messaging_core_subscriber_NativeSubscriber
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     messaging_core_subscriber_NativeSubscriber
 * Method:    nativeNew
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_messaging_core_subscriber_NativeSubscriber_nativeNew
  (JNIEnv *, jobject, jstring, jstring, jstring, jstring);

/*
 * Class:     messaging_core_subscriber_NativeSubscriber
 * Method:    poll
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_messaging_core_subscriber_NativeSubscriber_poll
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
