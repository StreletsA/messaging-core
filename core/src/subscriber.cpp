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

    std::cout << "SUBSCRIBER: SUBSCRIBER CONNECTING..." << '\n';
    
    sub_socket->connect(sub_connection_address);
    std::cout << "SUBSCRIBER: SUBSCRIBER CONNECTED!" << '\n';

    std::cout << "SUBSCRIBER: SUBSCRIBER SETTING OPTIONS..." << '\n';
    sub_socket->setsockopt(ZMQ_SUBSCRIBE, topic, strlen(topic));
    std::cout << "SUBSCRIBER: SUBSCRIBER TOPIC: " << topic << '\n';
    std::cout << "SUBSCRIBER: SUBSCRIBER OPTIONS SETTED!" << '\n';

    req_socket->setsockopt(ZMQ_REQ_RELAXED, 1);

    std::cout << "SUBSCRIBER: SUBSCRIBER CREATED!" << '\n';
    
    startup();
    start();
}

std::string Subscriber::poll()
{
    if (!messages.empty())
    {
        Message msg = messages.front();
        messages.pop();

        if (storage != nullptr)
        {
            storage->store_message(msg);
        }

        return msg.Serialize();
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

    std::cout << "SUBSCRIBER: DO RECOVERY: MESSAGE SIZE -> " << messages.size() << '\n';
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

    std::list<Message> missed_messages = recovery_response.get_messages(); 
    std::cout << "SUBSCRIBER: MISSED MESSAGES SIZE: " << missed_messages.size() << '\n';
    
    for (Message msg : missed_messages)
    {
        if (msg.get_sequence_number() > sequence_number)
        {
            sequence_number = msg.get_sequence_number();
            messages.push(msg);
        }
    }
    std::cout << "SUBSCRIBER: MESSAGES SIZE: " << messages.size() << '\n';
}

void Subscriber::thread_fn()
{

    std::cout << "SUBSCRIBER: SUBSCRIBER THREAD STARTED!" << '\n';
    
	while(true)
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
        std::string message_json = data;

        // Get message from json. Message data is still json format.
        if (message_json.size() > 0)
        {
            Message msg;
            msg.Deserialize(message_json);

            long msg_sequence_number = msg.get_sequence_number();

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
            messages.push(msg);
            sequence_number = msg_sequence_number;
        }

	}
}

