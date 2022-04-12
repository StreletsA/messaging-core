#include "datatypes.hpp"

/*
    #########################################################
    
    Message

    #########################################################
*/

Message::Message()
{
    this->uuid = generate_uuid_v4();
}

Message::Message(std::string data_json)
{
    this->uuid = generate_uuid_v4();
    this->data_json = data_json;
}

Message::Message
(
    long sequence_number,
    std::string uuid,
    std::string topic,
    long timestamp,
    MessageType message_type,
    bool needs_reply,
    std::string data_json
)
{
    this->sequence_number = sequence_number;
    this->uuid = generate_uuid_v4();
    this->topic = topic;
    this->timestamp = timestamp;
    this->message_type = message_type;
    this->needs_reply = needs_reply;
    this->data_json = data_json;
}

Message::Message
(
    std::string uuid,
    std::string topic,
    long timestamp,
    MessageType message_type,
    bool needs_reply,
    std::string data_json
)
{
    this->sequence_number = 0;
    this->uuid = generate_uuid_v4();
    this->topic = topic;
    this->timestamp = timestamp;
    this->message_type = message_type;
    this->needs_reply = needs_reply;
    this->data_json = data_json;
}

std::string Message::Serialize()
{

    cJSON *message = cJSON_CreateObject();

    cJSON *uuid_json = NULL;
    cJSON *topic_json = NULL;
    cJSON *sequence_number_json = NULL;
    cJSON *timestamp_json = NULL;
    cJSON *needs_reply_json = NULL;
    cJSON *message_type_json = NULL;
    cJSON *str_data_json = NULL;

    uuid_json = cJSON_CreateString(uuid.c_str());
    topic_json = cJSON_CreateString(topic.c_str());
    sequence_number_json = cJSON_CreateNumber(sequence_number);
    timestamp_json = cJSON_CreateNumber(timestamp);

    str_data_json = cJSON_CreateString(data_json.c_str());

    needs_reply_json = cJSON_CreateNumber(needs_reply ? 1 : 0);

    switch(message_type)
    {
        case MessageType::INFO:
            message_type_json = cJSON_CreateString("INFO");
            break;
        case MessageType::REJECT:
            message_type_json = cJSON_CreateString("REJECT");
            break;
        case MessageType::FAIL:
            message_type_json = cJSON_CreateString("FAIL");
            break;
        case MessageType::QUOTE:
            message_type_json = cJSON_CreateString("QUOTE");
            break;
        case MessageType::REPLY:
            message_type_json = cJSON_CreateString("REPLY");
            break;
        case MessageType::SUCCESS:
            message_type_json = cJSON_CreateString("SUCCESS");
            break;
    }

    
    cJSON_AddItemToObject(message, "uuid", uuid_json);
    cJSON_AddItemToObject(message, "topic", topic_json);
    cJSON_AddItemToObject(message, "sequence_number", sequence_number_json);
    cJSON_AddItemToObject(message, "timestamp", timestamp_json);
    cJSON_AddItemToObject(message, "needs_reply", needs_reply_json);
    cJSON_AddItemToObject(message, "message_type", message_type_json);
    cJSON_AddItemToObject(message, "data_json", str_data_json);

    return cJSON_Print(message);

}

cJSON* Message::SerializeToCJSON()
{

    cJSON *message = cJSON_CreateObject();

    cJSON *uuid_json = NULL;
    cJSON *topic_json = NULL;
    cJSON *sequence_number_json = NULL;
    cJSON *timestamp_json = NULL;
    cJSON *needs_reply_json = NULL;
    cJSON *message_type_json = NULL;
    cJSON *str_data_json = NULL;

    uuid_json = cJSON_CreateString(uuid.c_str());
    topic_json = cJSON_CreateString(topic.c_str());
    sequence_number_json = cJSON_CreateNumber(sequence_number);
    timestamp_json = cJSON_CreateNumber(timestamp);

    str_data_json = cJSON_CreateString(data_json.c_str());

    needs_reply_json = cJSON_CreateNumber(needs_reply ? 1 : 0);

    switch(message_type)
    {
        case MessageType::INFO:
            message_type_json = cJSON_CreateString("INFO");
            break;
        case MessageType::REJECT:
            message_type_json = cJSON_CreateString("REJECT");
            break;
        case MessageType::FAIL:
            message_type_json = cJSON_CreateString("FAIL");
            break;
        case MessageType::QUOTE:
            message_type_json = cJSON_CreateString("QUOTE");
            break;
        case MessageType::REPLY:
            message_type_json = cJSON_CreateString("REPLY");
            break;
        case MessageType::SUCCESS:
            message_type_json = cJSON_CreateString("SUCCESS");
            break;
    }

    cJSON_AddItemToObject(message, "uuid", uuid_json);
    cJSON_AddItemToObject(message, "topic", topic_json);
    cJSON_AddItemToObject(message, "sequence_number", sequence_number_json);
    cJSON_AddItemToObject(message, "timestamp", timestamp_json);
    cJSON_AddItemToObject(message, "needs_reply", needs_reply_json);
    cJSON_AddItemToObject(message, "message_type", message_type_json);
    cJSON_AddItemToObject(message, "data_json", str_data_json);


    return message;

}

bool Message::Deserialize(std::string json_string)
{

    cJSON *json = cJSON_Parse(json_string.c_str());
    
    uuid = cJSON_GetObjectItemCaseSensitive(json, "uuid")->valuestring;
    topic = cJSON_GetObjectItemCaseSensitive(json, "topic")->valuestring;
    sequence_number = cJSON_GetObjectItemCaseSensitive(json, "sequence_number")->valueint;
    timestamp = cJSON_GetObjectItemCaseSensitive(json, "timestamp")->valueint;

    needs_reply = cJSON_GetObjectItemCaseSensitive(json, "needs_reply")->valueint == 1;

    std::string msg_type_str = std::string(cJSON_GetObjectItemCaseSensitive(json, "message_type")->valuestring);

    if (msg_type_str == "INFO")
        message_type = MessageType::INFO;
    if (msg_type_str == "SUCCESS")
        message_type = MessageType::SUCCESS;
    if (msg_type_str == "REJECT")
        message_type = MessageType::REJECT;
    if (msg_type_str == "FAIL")
        message_type = MessageType::FAIL;
    if (msg_type_str == "QUOTE")
        message_type = MessageType::QUOTE;
    if (msg_type_str == "REPLY")
        message_type = MessageType::REPLY;

    data_json = std::string(cJSON_GetObjectItemCaseSensitive(json, "data_json")->valuestring);

    return true;

}

long Message::get_sequence_number()
{
    return sequence_number;
}

void Message::set_sequence_number(long sequence_number)
{
    this->sequence_number = sequence_number;
}

std::string Message::get_uuid()
{
    return uuid;
}

void Message::set_uuid(std::string uuid)
{
    this->uuid = uuid;
}

std::string Message::get_topic()
{
    return topic;
}

void Message::set_topic(std::string topic)
{
    this->topic = topic;
}

long Message::get_timestamp()
{
    return timestamp;
}

void Message::set_timestamp(long timestamp)
{
    this->timestamp = timestamp;
}

MessageType Message::get_message_type()
{
    return message_type;
}

void Message::set_message_type(MessageType message_type)
{
    this->message_type = message_type;
}

bool Message::get_needs_reply()
{
    return needs_reply;
}

void Message::set_needs_reply(bool needs_reply)
{
    this->needs_reply = needs_reply;
}

std::string Message::get_data()
{
    return data_json;
}

void Message::set_data(std::string data_json)
{
    this->data_json = data_json;
}

/*
    #########################################################
    
    StringData

    #########################################################
*/

StringData::StringData()
{

}

StringData::StringData(std::string str_data)
{
    this->str_data = str_data;
}

std::string StringData::Serialize()
{

    cJSON *string_data = cJSON_CreateObject();

    cJSON *data_json = NULL;

    data_json = cJSON_CreateString(str_data.c_str());

    cJSON_AddItemToObject(string_data, "string_data", data_json);

    return cJSON_Print(string_data);

}

bool StringData::Deserialize(std::string json_string)
{

    cJSON *json = cJSON_Parse(json_string.c_str());
    
    str_data = cJSON_GetObjectItemCaseSensitive(json, "string_data")->valuestring;

    return true;

}

std::string StringData::get_str_data()
{
    return str_data;
}

void StringData::set_str_data(std::string str_data)
{
    this->str_data = str_data;
}

/*
    #########################################################
    
    RecoveryRequest

    #########################################################
*/

std::string RecoveryRequest::Serialize()
{

    cJSON *recovery_request = cJSON_CreateObject();

    cJSON *startseqnum_json = NULL;
    cJSON *endseqnum_json = NULL;

    startseqnum_json = cJSON_CreateNumber(start_sequence_number);
    endseqnum_json = cJSON_CreateNumber(end_sequence_number);

    cJSON_AddItemToObject(recovery_request, "start_sequence_number", startseqnum_json);
    cJSON_AddItemToObject(recovery_request, "end_sequence_number", endseqnum_json);

    return cJSON_Print(recovery_request);

}

bool RecoveryRequest::Deserialize(std::string json_string)
{

    cJSON *json = cJSON_Parse(json_string.c_str());
    
    start_sequence_number = cJSON_GetObjectItemCaseSensitive(json, "start_sequence_number")->valueint;
    end_sequence_number = cJSON_GetObjectItemCaseSensitive(json, "end_sequence_number")->valueint;

    return true;

}

void RecoveryRequest::set_start_sequence_number(long start_sequence_number)
{
    this->start_sequence_number = start_sequence_number;
}

void RecoveryRequest::set_end_sequence_number(long end_sequence_number)
{
    this->end_sequence_number = end_sequence_number;
}

long RecoveryRequest::get_start_sequence_number()
{
    return start_sequence_number;
}

long RecoveryRequest::get_end_sequence_number()
{
    return end_sequence_number;
}

/*
    #########################################################
    
    RecoveryResponse

    #########################################################
*/

std::string RecoveryResponse::Serialize()
{
    cJSON *result = cJSON_CreateObject();
    cJSON *array = cJSON_AddArrayToObject(result, "messages");

    std::list<Message>::iterator it;
    for (it = messages.begin(); it != messages.end(); it++)
    {
        cJSON *message = it->SerializeToCJSON();
        
        cJSON_AddItemToArray(array, message);
    }
    return cJSON_Print(result);

}

bool RecoveryResponse::Deserialize(std::string json_string)
{
    cJSON *json = cJSON_Parse(json_string.c_str());
    cJSON *messages_array_json = cJSON_GetObjectItem(json, "messages");
    long size = cJSON_GetArraySize(messages_array_json);

    Message msg;

    for (long i = 0; i < size; i++)
    {
        cJSON *msg_json = cJSON_GetArrayItem(messages_array_json, i);
        msg.Deserialize(cJSON_Print(msg_json));

        messages.push_back(msg);
    }

    return true;

}

void RecoveryResponse::set_messages(std::list<Message> messages)
{
    this->messages = messages;
}

std::list<Message> RecoveryResponse::get_messages()
{
    return messages;
}
