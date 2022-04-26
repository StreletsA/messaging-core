#pragma once

#include <zmq.hpp>
#include <thread>
#include <queue>
#include <iostream>
#include <chrono>
#include <mutex>

#include "datatypes.hpp"
#include "persistent_storage.hpp"

class Subscriber;
class SubscriberThread;

class Subscriber
{

public:
    Subscriber(zmq::context_t *context, PersistentStorageInterface *storage, char *topic, const char* sub_connection_address, const char* req_connection_address);
    
    std::string get_topic();

    std::string poll();

    void start();
    void join();
    void detach();

private:

    bool isRecoveringNow = false;
    bool isSubConnected = false;

    zmq::context_t *ctx;
    zmq::socket_t *sub_socket;
    zmq::socket_t *req_socket;

    std::string topic;
    std::queue<Envelope> message_envelopes;
    std::thread *sub_thread;
    
    PersistentStorageInterface *storage;

    std::mutex g_mutex;

    long sequence_number;

    const char* sub_connection_address;
    const char* req_connection_address;
    std::string sub_connection_address_copy;
    std::string req_connection_address_copy;

    void startup();
    void sub_connecting_thread_fn();
    void recovery_time_controller_thread_fn();
    void thread_fn();
    void do_recovery(long startseqnum, long endseqnum);

};