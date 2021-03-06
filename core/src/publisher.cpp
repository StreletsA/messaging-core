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
    this->pub_connection_address = pub_connection_address;
    this->rep_connection_address = rep_connection_address;

    sequence_number = 0;

    std::thread *pub_binding_thread = new std::thread(&Publisher::pub_binding_thread_fn, this);
    std::thread *rep_binding_thread = new std::thread(&Publisher::rep_binding_thread_fn, this);
    start();

}

Publisher::Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address, PersistentStorageInterface *persistent_storage_interface)
{
    this->ctx = context;
    this->pub_socket = new zmq::socket_t(*ctx, zmq::socket_type::pub);
    this->rep_socket = new zmq::socket_t(*ctx, zmq::socket_type::rep);
    this->persistent_storage_interface = persistent_storage_interface;
    this->pub_connection_address = pub_connection_address;
    this->rep_connection_address = rep_connection_address;

    std::cout << "PUBLISHER: PUBLISHER CREATED! STARTING THREAD" << '\n';
    
    sequence_number = persistent_storage_interface->get_sequence_number();

    std::thread *pub_binding_thread = new std::thread(&Publisher::pub_binding_thread_fn, this);
    std::thread *rep_binding_thread = new std::thread(&Publisher::rep_binding_thread_fn, this);
    start();
}

Publisher::Publisher(zmq::context_t *context, const char *pub_connection_address, const char *rep_connection_address, PersistentStorageInterface *persistent_storage_interface, int HWM)
{
    this->ctx = context;
    this->pub_socket = new zmq::socket_t(*ctx, zmq::socket_type::pub);
    this->rep_socket = new zmq::socket_t(*ctx, zmq::socket_type::rep);
    this->persistent_storage_interface = persistent_storage_interface;
    this->pub_connection_address = pub_connection_address;
    this->rep_connection_address = rep_connection_address;

    // Setting HWM

    std::cout << "PUBLISHER: PUBLISHER CREATED! STARTING THREAD" << '\n';

    sequence_number = persistent_storage_interface->load_sequence_number();

    std::thread *pub_binding_thread = new std::thread(&Publisher::pub_binding_thread_fn, this);
    std::thread *rep_binding_thread = new std::thread(&Publisher::rep_binding_thread_fn, this);
    start();
}

void Publisher::pub_binding_thread_fn(){

    while (true)
    {
        while (!isPubBinded)
        {
            try
            {
                pub_socket->bind(pub_connection_address);

                isPubBinded = true;

            }
            catch(const zmq::error_t& e)
            {
                std::cerr << e.what() << '\n';
            }

            std::this_thread::sleep_for (std::chrono::microseconds(250000));

            
        }

        std::this_thread::sleep_for (std::chrono::microseconds(250000));

    }
    

}

void Publisher::rep_binding_thread_fn(){

    while (true)
    {
        while (!isRepBinded)
        {
            try
            {
                rep_socket->bind(rep_connection_address);

                isRepBinded = true;

            }
            catch(const zmq::error_t& e)
            {
                std::cerr << e.what() << '\n';
            }

            std::this_thread::sleep_for (std::chrono::microseconds(250000));

            
        }

        std::this_thread::sleep_for (std::chrono::microseconds(250000));

    }
    

}

void Publisher::publish(Envelope message_envelope)
{
    g_mutex.lock();
    incoming_message_envelope_queue.push(message_envelope);
    g_mutex.unlock();
}

void Publisher::publish(std::string message_envelope_json)
{
    Envelope msg_envelope;
    msg_envelope.Deserialize(message_envelope_json);
    publish(msg_envelope);
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

        while(!incoming_message_envelope_queue.empty())
        {
            
            try
            {

                // Get message from queue
                Envelope msg_envelope = incoming_message_envelope_queue.front();
                incoming_message_envelope_queue.pop();
                sequence_number++;

                if(persistent_storage_interface != nullptr)
                {
                    // Setting sequence number from storage
                    msg_envelope.set_sequence_number(persistent_storage_interface->get_sequence_number() + 1);
                    persistent_storage_interface->store_message_envelope(msg_envelope);
                }
                else
                {
                    // Setting sequence number from our publisher seq num
                    msg_envelope.set_sequence_number(sequence_number);
                }

                std::cout << "PUBLISHER: MSG SEQ_NUM -> " << msg_envelope.get_sequence_number() << '\n';

                // Create string data for sending: TOPIC + DATA
                std::string topic = msg_envelope.get_topic();
                std::string str_msg_envelope = msg_envelope.Serialize();

                // Create message from string data
                zmq::message_t message_topic(topic.size());
                zmq::message_t message_envelope(str_msg_envelope.size());
                memcpy (message_topic.data(), topic.data(), topic.size());
                memcpy (message_envelope.data(), str_msg_envelope.data(), str_msg_envelope.size());

                // Send message
                pub_socket->send (message_topic, zmq::send_flags::sndmore);
                pub_socket->send (message_envelope);

                // HACK: publishing interval for java subscriber
                std::this_thread::sleep_for (std::chrono::microseconds(500000));
                    
                

            }
            catch(const zmq::error_t& e)
            {
                isPubBinded = false;
                std::cerr << e.what() << '\n';
                std::this_thread::sleep_for (std::chrono::microseconds(250000));
            }
        }
    
        while(incoming_message_envelope_queue.empty())
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

        try
        {
            zmq::message_t msg_envelope;
            rep_socket->recv(&msg_envelope);
            
            // Get string data from zmq message
            std::string data = std::string(static_cast<char*>(msg_envelope.data()), msg_envelope.size());

            RecoveryRequest recovery_request;
            recovery_request.Deserialize(data);

            std::cout << "PUBLISHER: RECEIVED RECOVERY REQUEST: " << recovery_request.get_start_sequence_number() << " : " << recovery_request.get_end_sequence_number() << '\n';

            std::list<Envelope> message_envelopes;

            long recovery_start_seq_num = recovery_request.get_start_sequence_number();
            long recovery_end_seq_num = recovery_request.get_end_sequence_number();

            if (persistent_storage_interface != nullptr)
            {
                message_envelopes = persistent_storage_interface->get_message_envelopes(recovery_start_seq_num, recovery_end_seq_num);
                std::cout << "PUBLISHER: MISSED MESSAGES GOT FROM STORAGE! SIZE: " << message_envelopes.size() << '\n';
            }

            RecoveryResponse recovery_response;
            recovery_response.set_message_envelopes(message_envelopes);

            std::cout << "PUBLISHER: RECOVERY RESPONSE MESSAGES SIZE: " << recovery_response.get_message_envelopes().size() << '\n';
            
            std::string response_json = recovery_response.Serialize();

            zmq::message_t response_msg(response_json);
            rep_socket->send(response_msg);

            std::cout << "RECOVERY RESPONSE SENT! "  << '\n';
        }
        catch(const zmq::error_t& e)
        {
            isRepBinded = false;
            std::cerr << e.what() << '\n';
            std::this_thread::sleep_for (std::chrono::microseconds(250000));
        }

    }

}

std::queue<Envelope> *Publisher::get_incoming_message_envelope_queue()
{
    return &incoming_message_envelope_queue;
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