#include "messaging_core_subscriber_NativeSubscriber.h"
#include <string>
#include <iostream>
#include "../src/persistent_storage.hpp"
#include "../src/subscriber.hpp"
#include "../src/network_params.hpp"
#include "utils/utils.h"
#include <zmq.hpp>

using namespace std;


JNIEXPORT jlong JNICALL Java_messaging_core_subscriber_NativeSubscriber_nativeNew
  (JNIEnv *env, jobject obj, jstring topic, jstring sub_connection_address, jstring req_connection_address)
  {
      zmq::context_t *ctx = Context::getInstance().get_context();
      TestSubscriberPersistenceStorage *tsps = new TestSubscriberPersistenceStorage();

      string arg_topic = jstring2string(env, topic);
      string arg_sub = jstring2string(env, sub_connection_address);
      string arg_req = jstring2string(env, req_connection_address);

      Subscriber *subscriber = new Subscriber(ctx, tsps, (char*)arg_topic.c_str(), arg_sub.c_str(), arg_req.c_str());

      return (long) subscriber;
  }
  
JNIEXPORT jstring JNICALL Java_messaging_core_subscriber_NativeSubscriber_poll
  (JNIEnv *env, jobject obj, jlong pointer)
  {
  	Subscriber *subscriber = (Subscriber *) pointer;
  	std::string message_json = subscriber->poll();

  	//std::cout << "JNI SUBSCRIBER: MESSAGE POLLED: " << message_json << '\n';
  	
  	return env->NewStringUTF(message_json.c_str());
  }

JNIEXPORT void JNICALL Java_messaging_core_subscriber_Subscriber_join
  (JNIEnv *, jobject, jlong pointer)
  {

    Subscriber *subscriber = (Subscriber *) pointer;
    subscriber->join();

  }
