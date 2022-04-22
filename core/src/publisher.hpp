#include <zmq.hpp>
#include <thread>
#include <queue>
#include <iostream>
#include <chrono>
#include <atomic>
#include <mutex>

#include "datatypes.hpp"
#include "persistent_storage.hpp"


class Publisher
{

public:
    Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address);
    Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address, PersistentStorageInterface *persistent_storage_interface);
    Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address, PersistentStorageInterface *persistent_storage_interface, int HWM);

    void publish(Envelope message_envelope);
    void publish(std::string message_json);
    void start();
    void join();
    void set_persistent_storage_interface(PersistentStorageInterface *persistent_storage_interface);

    std::queue<Envelope> *get_incoming_message_envelope_queue();
    PersistentStorageInterface *get_persistent_storage_interface();
    long *get_sequence_number();
    
private:
    long sequence_number;

    zmq::context_t *ctx;
    zmq::socket_t *pub_socket;
    zmq::socket_t *rep_socket;

    std::mutex g_mutex;

    std::queue<Envelope> incoming_message_envelope_queue;

    std::thread *pub_thread;
    std::thread *rep_thread;

    void pub_thread_fn();
    void rep_thread_fn();

    PersistentStorageInterface *persistent_storage_interface;

};
