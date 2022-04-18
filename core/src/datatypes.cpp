#include "datatypes.hpp"

/*
    #########################################################
    
    Message

    #########################################################
*/

Message::Message()
{
    this->uuid = generate_uuid_v4();

    sequence_number = 0;
    timestamp = std::time(nullptr);
    topic = "TRANSACTION";
    body = "";
}

Message::Message(std::string body)
{
    this->uuid = generate_uuid_v4();
    this->body = body;

    sequence_number = 0;
    timestamp = std::time(nullptr);
    topic = "TRANSACTION";
}

Message::Message
(
    std::string topic,
    std::string uuid,
    long sequence_number,
    long timestamp,
    std::string body
)
{
    this->sequence_number = sequence_number;
    this->uuid = generate_uuid_v4();
    this->topic = topic;
    this->timestamp = timestamp;
    this->body = body;
}

Message::Message
(
    std::string topic,
    std::string uuid,
    long timestamp,
    std::string body
)
{
    this->sequence_number = 0;
    this->uuid = generate_uuid_v4();
    this->topic = topic;
    this->timestamp = timestamp;
    this->body = body;
}

std::string Message::Serialize()
{

    cJSON *message = cJSON_CreateObject();

    cJSON *uuid_json = NULL;
    cJSON *topic_json = NULL;
    cJSON *sequence_number_json = NULL;
    cJSON *timestamp_json = NULL;
    cJSON *body_json = NULL;

    uuid_json = cJSON_CreateString(uuid.c_str());
    topic_json = cJSON_CreateString(topic.c_str());
    sequence_number_json = cJSON_CreateNumber(sequence_number);
    timestamp_json = cJSON_CreateNumber(timestamp);
    body_json = cJSON_CreateString(body.c_str());
    
    cJSON_AddItemToObject(message, "uuid", uuid_json);
    cJSON_AddItemToObject(message, "topic", topic_json);
    cJSON_AddItemToObject(message, "sequence_number", sequence_number_json);
    cJSON_AddItemToObject(message, "timestamp", timestamp_json);
    cJSON_AddItemToObject(message, "body", body_json);

    return cJSON_Print(message);

}

cJSON* Message::SerializeToCJSON()
{

    cJSON *message = cJSON_CreateObject();

    cJSON *uuid_json = NULL;
    cJSON *topic_json = NULL;
    cJSON *sequence_number_json = NULL;
    cJSON *timestamp_json = NULL;
    cJSON *body_json = NULL;

    uuid_json = cJSON_CreateString(uuid.c_str());
    topic_json = cJSON_CreateString(topic.c_str());
    sequence_number_json = cJSON_CreateNumber(sequence_number);
    timestamp_json = cJSON_CreateNumber(timestamp);

    body_json = cJSON_CreateString(body.c_str());

    cJSON_AddItemToObject(message, "uuid", uuid_json);
    cJSON_AddItemToObject(message, "topic", topic_json);
    cJSON_AddItemToObject(message, "sequence_number", sequence_number_json);
    cJSON_AddItemToObject(message, "timestamp", timestamp_json);
    cJSON_AddItemToObject(message, "body", body_json);


    return message;

}

bool Message::Deserialize(std::string json_string)
{

    cJSON *json = cJSON_Parse(json_string.c_str());
    
    uuid = cJSON_GetObjectItemCaseSensitive(json, "uuid")->valuestring;
    topic = cJSON_GetObjectItemCaseSensitive(json, "topic")->valuestring;
    sequence_number = cJSON_GetObjectItemCaseSensitive(json, "sequence_number")->valueint;
    timestamp = cJSON_GetObjectItemCaseSensitive(json, "timestamp")->valueint;

    body = std::string(cJSON_GetObjectItemCaseSensitive(json, "body")->valuestring);

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

std::string Message::get_body()
{
    return body;
}

void Message::set_body(std::string body)
{
    this->body = body;
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
