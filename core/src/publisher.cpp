#include "publisher.hpp"

/*
    #########################################################
    
    Publisher

    #########################################################
*/

Publisher::Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address)
{
    this->ctx = context;
    this->pub_socket = new zmq::socket_t(*ctx, zmq::socket_type::pub);
    this->rep_socket = new zmq::socket_t(*ctx, zmq::socket_type::rep);

    sequence_number = 0;

    pub_socket->bind(pub_connection_address);
    rep_socket->bind(rep_connection_address);

    std::cout << "PUBLISHER: PUBLISHER CREATED! STARTING THREAD" << '\n';

    start();

}

Publisher::Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address, PersistentStorageInterface *persistent_storage_interface)
{
    this->ctx = context;
    this->pub_socket = new zmq::socket_t(*ctx, zmq::socket_type::pub);
    this->rep_socket = new zmq::socket_t(*ctx, zmq::socket_type::rep);
    this->persistent_storage_interface = persistent_storage_interface;

    pub_socket->bind(pub_connection_address);
    rep_socket->bind(rep_connection_address);

    std::cout << "PUBLISHER: PUBLISHER CREATED! STARTING THREAD" << '\n';
    
    sequence_number = persistent_storage_interface->get_sequence_number();

    start();
}

Publisher::Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address, PersistentStorageInterface *persistent_storage_interface, int HWM)
{
    this->ctx = context;
    this->pub_socket = new zmq::socket_t(*ctx, zmq::socket_type::pub);
    this->rep_socket = new zmq::socket_t(*ctx, zmq::socket_type::rep);
    this->persistent_storage_interface = persistent_storage_interface;

    // Setting HWM

    pub_socket->bind(pub_connection_address);
    rep_socket->bind(rep_connection_address);

    std::cout << "PUBLISHER: PUBLISHER CREATED! STARTING THREAD" << '\n';

    sequence_number = persistent_storage_interface->load_sequence_number();

    start();
}

void Publisher::publish(Message message)
{
    g_mutex.lock();
    incoming_message_queue.push(message);
    g_mutex.unlock();
}

void Publisher::publish(std::string message_json)
{
    Message msg;
    msg.Deserialize(message_json);
    publish(msg);
}

void Publisher::start()
{
    pub_thread = new std::thread(&Publisher::pub_thread_fn, this);
    rep_thread = new std::thread(&Publisher::rep_thread_fn, this);
}

void Publisher::pub_thread_fn()
{
    
    std::cout << "PUBLISHER: PUBLISHER THREAD STARTED!" << '\n';
    
    while(true)
    {

        while(!incoming_message_queue.empty())
        {
            
            // Get message from queue
            Message msg = incoming_message_queue.front();
            incoming_message_queue.pop();
            sequence_number++;

            if(persistent_storage_interface != nullptr)
            {
                // Setting sequence number from storage
                msg.set_sequence_number(persistent_storage_interface->get_sequence_number() + 1);
                persistent_storage_interface->store_message(msg);
            }
            else
            {
                // Setting sequence number from our publisher seq num
                msg.set_sequence_number(sequence_number);
            }

            std::cout << "PUBLISHER: MSG SEQ_NUM -> " << msg.get_sequence_number() << '\n';

            // Create string data for sending: TOPIC + DATA
            std::string topic = msg.get_topic();
            std::string str_msg = msg.Serialize();

            // Create message from string data
            zmq::message_t message_topic(topic.size());
            zmq::message_t message(str_msg.size());
            memcpy (message_topic.data(), topic.data(), topic.size());
            memcpy (message.data(), str_msg.data(), str_msg.size());

            // Send message
            pub_socket->send (message_topic, zmq::send_flags::sndmore);
            pub_socket->send (message);

            // HACK: publishing interval for java subscriber
            std::this_thread::sleep_for (std::chrono::microseconds(500000));
            
        }

        while(incoming_message_queue.empty())
        {

            //incoming_message_queue.wait();

        }

    }

}

void Publisher::rep_thread_fn()
{

    while(true)
    {
        // {
        //     "start_sequence_number": N,
        //     "end_sequence_number" : sequence(message)
        // }
        // Get recovery request
        // (N, sequence(message))
        // N - number of last message client had (before message that publisher sent)
        // sequence(message) - number of last message that publisher sent to client


        zmq::message_t msg;
        rep_socket->recv(&msg);
        
        // Get string data from zmq message
        std::string data = std::string(static_cast<char*>(msg.data()), msg.size());

        RecoveryRequest recovery_request;
        recovery_request.Deserialize(data);

        std::cout << "PUBLISHER: RECEIVED RECOVERY REQUEST: " << recovery_request.get_start_sequence_number() << " : " << recovery_request.get_end_sequence_number() << '\n';

        std::list<Message> messages;

        if (persistent_storage_interface != nullptr)
        {
            messages = persistent_storage_interface->get_messages(recovery_request.get_start_sequence_number(), recovery_request.get_end_sequence_number());
            std::cout << "PUBLISHER: MISSED MESSAGES GOT FROM STORAGE! SIZE: " << messages.size() << '\n';
        }

        RecoveryResponse recovery_response;
        recovery_response.set_messages(messages);

        std::cout << "PUBLISHER: RECOVERY RESPONSE MESSAGES SIZE: " << recovery_response.get_messages().size() << '\n';
        
        std::string response_json = recovery_response.Serialize();

        zmq::message_t response_msg(response_json);
        rep_socket->send(response_msg);

        std::cout << "RECOVERY RESPONSE SENT! "  << '\n';

    }

}

std::queue<Message> *Publisher::get_incoming_message_queue()
{
    return &incoming_message_queue;
}

PersistentStorageInterface *Publisher::get_persistent_storage_interface()
{
    return persistent_storage_interface;
}

void Publisher::set_persistent_storage_interface(PersistentStorageInterface *persistent_storage_interface)
{
    this->persistent_storage_interface = persistent_storage_interface;
}

long *Publisher::get_sequence_number()
{
    return &sequence_number;
}

void Publisher::join()
{
    pub_thread->join();
    rep_thread->join();
}