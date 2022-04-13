#include "messaging_core_publisher_Publisher.h"
#include <string>
#include <iostream>
#include "../src/datatypes.hpp"
#include "../src/persistent_storage.hpp"
#include "../src/publisher.hpp"
#include "../src/network_params.hpp"
#include <zmq.hpp>
#include "utils/utils.h"

JNIEXPORT jlong JNICALL Java_messaging_core_publisher_Publisher_nativeNew
  (JNIEnv *env, jobject obj, jstring json_params, jstring pub_addr, jstring rep_addr)
  {
    zmq::context_t *ctx = Context::getInstance().get_context();
    
    //std::string arg_json_params = jstring2string(env, json_params);
    std::string arg_json_params = "{\"databaseType\": \"POSTGRESQL\", \"dbName\": \"messaging_core\", \"user\": \"postgres\", \"password\": \"postgres\", \"hostAddress\": \"127.0.0.1\", \"port\": \"5432\"}";

    std::string arg_pub = jstring2string(env, pub_addr);
    std::string arg_rep = jstring2string(env, rep_addr);
    
    //std::cout << "JNI PUBLISHER: JSON_PARAMS -> " << arg_json_params << '\n';
	  PersistentStorageInterface *tps = PersistentStorage::getPersistentStorageInterface(arg_json_params);
    //TestPersistenceStorage *tps = new TestPersistenceStorage();
    Publisher *publisher = new Publisher(ctx, arg_pub.c_str(), arg_rep.c_str(), tps);

    return (long) publisher;
  }

 JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_publish
  (JNIEnv *env, jobject obj, jlong pointer, jlong seq_num, jstring uuid, jstring topic, jlong timestamp, jstring msg_type, jboolean needs_reply, jstring data_json)
  {
  	Publisher *publisher = (Publisher*) pointer;
  	Message *msg = new Message(seq_num, jstring2string(env, uuid), jstring2string(env, topic), timestamp, MessageType::INFO, needs_reply, jstring2string(env, data_json));
  	publisher->publish(*msg);
  }
  
 JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_publishByJson
  (JNIEnv *env, jobject obj, jlong pointer, jstring json)
  {
  	Publisher *publisher = (Publisher*) pointer;
  	//Message *msg = new Message();
  	//msg->Deserialize(jstring2string(env, json));
  	publisher->publish(jstring2string(env, json));
  }

  JNIEXPORT void JNICALL Java_messaging_core_publisher_Publisher_join
  (JNIEnv *, jobject, jlong pointer)
  {

    Publisher *publisher = (Publisher*) pointer;
    publisher->join();

  }
