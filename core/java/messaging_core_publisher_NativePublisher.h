/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class messaging_core_publisher_NativePublisher */

#ifndef _Included_messaging_core_publisher_NativePublisher
#define _Included_messaging_core_publisher_NativePublisher
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     messaging_core_publisher_NativePublisher
 * Method:    nativeNew
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_messaging_core_publisher_NativePublisher_nativeNew
  (JNIEnv *, jobject, jstring, jstring, jstring);

/*
 * Class:     messaging_core_publisher_NativePublisher
 * Method:    publish
 * Signature: (JLjava/lang/String;Ljava/lang/String;JJZLjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_messaging_core_publisher_NativePublisher_publish
  (JNIEnv *, jobject, jlong, jstring, jstring, jlong, jlong, jboolean, jstring, jstring);

/*
 * Class:     messaging_core_publisher_NativePublisher
 * Method:    publishByJson
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_messaging_core_publisher_NativePublisher_publishByJson
  (JNIEnv *, jobject, jlong, jstring);

#ifdef __cplusplus
}
#endif
#endif
