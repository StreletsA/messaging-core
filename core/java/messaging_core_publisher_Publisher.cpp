#include "messaging_core_publisher_Publisher.h"
#include <string>
#include <iostream>
#include "../src/datatypes.hpp"
#include "../src/persistent_storage.hpp"
#include "../src/publisher.hpp"
#include <zmq.hpp>
#include "utils/utils.h"

JNIEXPORT jlong JNICALL Java_messaging_core_publisher_Publisher_nativeNew
  (JNIEnv *env, jobject obj, jstring pub_addr, jstring rep_addr)
  {
    zmq::context_t *ctx = new zmq::context_t(2);
    std::string arg_pub = jstring2string(env, pub_addr);
    std::string arg_rep = jstring2string(env, rep_addr);

    TestPersistenceStorage *tps = new TestPersistenceStorage();
    Publisher *publisher = new Publisher(ctx, arg_pub.c_str(), arg_rep.c_str(), tps);

    return (long) publisher;
  }
/*
JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_publish__JLjava_lang_String_2
  (JNIEnv *env, jobject obj, jlong pointer, jstring message_json)
  {
    Publisher *publisher = (Publisher*) pointer;

    //Message *msg = new Message(seq_num, jstring2string(env, uuid), jstring2string(env, topic), timestamp, MessageType::INFO, needs_reply, jstring2string(env, data_json));
    
    std::cout << "JNI PUBLISHER: MESSAGE_JSON GOT: " << jstring2string(env, message_json) << '\n';
    
    Message *msg = new Message();
    msg->Deserialize(jstring2string(env, message_json));

    publisher->publish(*msg);
  }
  */
 JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_publish
  (JNIEnv *env, jobject obj, jlong pointer, jlong seq_num, jstring uuid, jstring topic, jlong timestamp, jstring msg_type, jboolean needs_reply, jstring data_json)
  {
  	Publisher *publisher = (Publisher*) pointer;
  	Message *msg = new Message(seq_num, jstring2string(env, uuid), jstring2string(env, topic), timestamp, MessageType::INFO, needs_reply, jstring2string(env, data_json));
  	publisher->publish(*msg);
  }

JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_start
  (JNIEnv *, jobject, jlong pointer)
  {
    Publisher *publisher = (Publisher*) pointer;
    publisher->start();
  }

JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_join
  (JNIEnv *, jobject, jlong pointer)
  {
    Publisher *publisher = (Publisher*) pointer;
    publisher->join();
  }
