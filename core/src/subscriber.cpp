#include "subscriber.hpp"

Subscriber::Subscriber(zmq::context_t *context, PersistentStorageInterface *storage, char *topic, const char *sub_connection_address, const char *req_connection_address)
{
    this->ctx = context;
    this->storage = storage;
    this->sub_socket = new zmq::socket_t(*context, zmq::socket_type::sub);
    this->req_socket = new zmq::socket_t(*context, zmq::socket_type::req);
    this->sub_connection_address = sub_connection_address;
    this->req_connection_address = req_connection_address;
    this->topic = std::string(topic);

    sequence_number = storage->load_sequence_number();

    std::cout << "SUBSCRIBER: SUBSCRIBER CREATING..." << '\n';

    std::thread* sub_connecting_thread = new std::thread(&Subscriber::sub_connecting_thread_fn, this);
    std::thread* req_connecting_thread = new std::thread(&Subscriber::req_connecting_thread_fn, this);
    startup();
    start();
}

void Subscriber::sub_connecting_thread_fn()
{

    while (true)
    {

        while (!isSubConnected)
        {

            try
            {
                std::cout << "SUBSCRIBER: SUBSCRIBER CONNECTING TO " << sub_connection_address << '\n';
                sub_socket->connect(sub_connection_address);
                std::cout << "SUBSCRIBER: SUBSCRIBER CONNECTED!" << '\n';
                sub_socket->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), strlen(topic.c_str()));
                std::cout << "SUBSCRIBER: SUBSCRIBER TOPIC: " << topic << '\n';
                isSubConnected = true;
            }
            catch(const zmq::error_t e)
            {
                std::cerr << e.what() << '\n';
            }

            std::this_thread::sleep_for (std::chrono::microseconds(250000));
            
        }

        std::this_thread::sleep_for (std::chrono::microseconds(250000));

    }

}

void Subscriber::req_connecting_thread_fn()
{

    while (true)
    {

        while (!isReqConnected)
        {

            try
            {
                req_socket->setsockopt(ZMQ_REQ_RELAXED, 1);
                isReqConnected = true;
            }
            catch(const zmq::error_t e)
            {
                std::cerr << e.what() << '\n';
                std::this_thread::sleep_for (std::chrono::microseconds(250000));
            }
            
        }

        std::this_thread::sleep_for (std::chrono::microseconds(250000));

    }

}

std::string Subscriber::poll()
{
    if (!message_envelopes.empty())
    {
        Envelope msg_envelope = message_envelopes.front();
        message_envelopes.pop();

        if (storage != nullptr)
        {
            storage->store_message_envelope(msg_envelope);
        }

        return msg_envelope.Serialize();
    }
    

    return "";
}

void Subscriber::startup()
{
    std::cout << "SUBSCRIBER: STARTUP TSK STARTED!" << '\n';

    // Synchronization
    // We couldn't worry about this, because server understand what we want
    do_recovery(sequence_number, 0);

    std::cout << "SUBSCRIBER: STARTUP TSK FINISHED!" << '\n';

}

void Subscriber::start()
{
    sub_thread = new std::thread(&Subscriber::thread_fn, this);
}

void Subscriber::join()
{
    sub_thread->join();
}

void Subscriber::detach()
{
    sub_thread->detach();
}

void Subscriber::do_recovery(long startseqnum, long endseqnum)
{

    try
    {
        
        std::cout << "SUBSCRIBER: DO RECOVERY: MESSAGE SIZE -> " << message_envelopes.size() << '\n';
        std::cout << "SUBSCRIBER: DO RECOVERY " << startseqnum << " : " << endseqnum << '\n';

        if (startseqnum > endseqnum)
        {
            return;
        }

        RecoveryRequest recovery_request;
        recovery_request.set_start_sequence_number(startseqnum);
        recovery_request.set_end_sequence_number(endseqnum);

        req_socket->connect(req_connection_address);

        std::string request_string = recovery_request.Serialize();

        // Create message from string data
        zmq::message_t message(request_string.size());
        memcpy (message.data(), request_string.data(), request_string.size());

        // Send message
        req_socket->send (message);

        // Receive message
        zmq::message_t resp_message;
        req_socket->recv(&resp_message);

        // Get string data from zmq message
        std::string resp_data = std::string(static_cast<char*>(resp_message.data()), resp_message.size());

        //std::cout << "RESP DATA: " << resp_data << '\n';

        RecoveryResponse recovery_response;
        recovery_response.Deserialize(resp_data);

        std::list<Envelope> missed_message_envelopes = recovery_response.get_message_envelopes(); 
        std::cout << "SUBSCRIBER: MISSED MESSAGES SIZE: " << missed_message_envelopes.size() << '\n';
        
        for (Envelope msg_envelope : missed_message_envelopes)
        {
            if (msg_envelope.get_sequence_number() > sequence_number)
            {
                sequence_number = msg_envelope.get_sequence_number();
                message_envelopes.push(msg_envelope);
            }
        }
        std::cout << "SUBSCRIBER: MESSAGES SIZE: " << message_envelopes.size() << '\n';

    }
    catch(const zmq::error_t e)
    {
        isReqConnected = false;
        std::cerr << e.what() << '\n';
        std::this_thread::sleep_for (std::chrono::microseconds(250000));
    }
}

void Subscriber::thread_fn()
{

    std::cout << "SUBSCRIBER: SUBSCRIBER THREAD STARTED!" << '\n';
    
	while(true)
	{
        
        try
        {
            // Create a received zmq message
            zmq::message_t message_topic;
            zmq::message_t message;

            // Receive the zmq message
            sub_socket->recv(&message_topic);
            sub_socket->recv(&message);

            // Get string data from zmq message
            std::string topic = std::string(static_cast<char*>(message_topic.data()), message_topic.size());
            std::string data = std::string(static_cast<char*>(message.data()), message.size());
            std::cout << "SUBSCRIBER: RECV TOPIC -> " << topic << '\n';
            // Delete topic name from string data and get json
            //std::string message_json = data.substr(topic.size());
            std::string message_envelope_json = data;

            // Get message from json. Message data is still json format.
            if (message_envelope_json.size() > 0)
            {
                Envelope msg_envelope;
                msg_envelope.Deserialize(message_envelope_json);

                long msg_sequence_number = msg_envelope.get_sequence_number();

                std::cout << "SUBSCRIBER: RECEIVED MESSAGE WITH SEQ_NUM: " << msg_sequence_number << '\n';

                if (msg_sequence_number > sequence_number + 1)
                {
                    do_recovery(sequence_number + 1, msg_sequence_number);
                    continue;
                }
                if (msg_sequence_number < sequence_number + 1)
                {
                    continue;
                }

                // Add message into messages queue
                message_envelopes.push(msg_envelope);
                sequence_number = msg_sequence_number;
            }
        }
        catch(const zmq::error_t e)
        {
            isSubConnected = false;
            std::cerr << e.what() << '\n';
            std::this_thread::sleep_for (std::chrono::microseconds(250000));
        }
        
        

	}
}

