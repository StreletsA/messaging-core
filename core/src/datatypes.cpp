#include "datatypes.hpp"

/*
    #########################################################
    
    Message

    #########################################################
*/

Envelope::Envelope()
{
    this->uuid = generate_uuid_v4();

    sequence_number = 0;
    timestamp = std::time(nullptr);
    success = true;
    error = "";
    topic = "";
    body = "";
}

Envelope::Envelope(std::string body)
{
    this->uuid = generate_uuid_v4();
    this->body = body;

    success = true;
    error = "";
    sequence_number = 0;
    timestamp = std::time(nullptr);
    topic = "";
}

Envelope::Envelope
(
    std::string topic,
    std::string uuid,
    long sequence_number,
    long timestamp,
    bool success,
    std::string error,
    std::string body
)
{
    this->sequence_number = sequence_number;
    this->uuid = uuid;
    this->topic = topic;
    this->timestamp = timestamp;
    this->success = success;
    this->error = error;
    this->body = body;
}

Envelope::Envelope
(
    std::string topic,
    std::string uuid,
    long timestamp,
    bool success,
    std::string error,
    std::string body
)
{
    this->sequence_number = 0;
    this->uuid = uuid;
    this->topic = topic;
    this->timestamp = timestamp;
    this->success = success;
    this->error = error;
    this->body = body;
}

std::string Envelope::Serialize()
{

    cJSON *message = cJSON_CreateObject();

    cJSON *uuid_json = NULL;
    cJSON *topic_json = NULL;
    cJSON *sequence_number_json = NULL;
    cJSON *timestamp_json = NULL;
    cJSON *success_json = NULL;
    cJSON *error_json = NULL;
    cJSON *body_json = NULL;

    uuid_json = cJSON_CreateString(uuid.c_str());
    topic_json = cJSON_CreateString(topic.c_str());
    sequence_number_json = cJSON_CreateNumber(sequence_number);
    timestamp_json = cJSON_CreateNumber(timestamp);
    success_json = cJSON_CreateBool(success);
    error_json = cJSON_CreateString(error.c_str());
    body_json = cJSON_CreateString(body.c_str());
    
    cJSON_AddItemToObject(message, "uuid", uuid_json);
    cJSON_AddItemToObject(message, "topic", topic_json);
    cJSON_AddItemToObject(message, "sequenceNumber", sequence_number_json);
    cJSON_AddItemToObject(message, "timestamp", timestamp_json);
    cJSON_AddItemToObject(message, "success", success_json);
    cJSON_AddItemToObject(message, "error", error_json);
    cJSON_AddItemToObject(message, "body", body_json);

    return cJSON_Print(message);

}

cJSON* Envelope::SerializeToCJSON()
{

    cJSON *message = cJSON_CreateObject();

    cJSON *uuid_json = NULL;
    cJSON *topic_json = NULL;
    cJSON *sequence_number_json = NULL;
    cJSON *timestamp_json = NULL;
    cJSON *success_json = NULL;
    cJSON *error_json = NULL;
    cJSON *body_json = NULL;

    uuid_json = cJSON_CreateString(uuid.c_str());
    topic_json = cJSON_CreateString(topic.c_str());
    sequence_number_json = cJSON_CreateNumber(sequence_number);
    timestamp_json = cJSON_CreateNumber(timestamp);
    success_json = cJSON_CreateBool(success);
    error_json = cJSON_CreateString(error.c_str());
    body_json = cJSON_CreateString(body.c_str());

    cJSON_AddItemToObject(message, "uuid", uuid_json);
    cJSON_AddItemToObject(message, "topic", topic_json);
    cJSON_AddItemToObject(message, "sequenceNumber", sequence_number_json);
    cJSON_AddItemToObject(message, "timestamp", timestamp_json);
    cJSON_AddItemToObject(message, "success", success_json);
    cJSON_AddItemToObject(message, "error", error_json);
    cJSON_AddItemToObject(message, "body", body_json);


    return message;

}

bool Envelope::Deserialize(std::string json_string)
{

    cJSON *json = cJSON_Parse(json_string.c_str());
    
    uuid = cJSON_GetObjectItem(json, "uuid")->valuestring;
    topic = cJSON_GetObjectItem(json, "topic")->valuestring;
    sequence_number = cJSON_GetObjectItem(json, "sequenceNumber")->valueint;
    timestamp = cJSON_GetObjectItem(json, "timestamp")->valueint;
    success = cJSON_GetObjectItem(json, "success")->valueint == 1;
    error = cJSON_GetObjectItem(json, "error")->valuestring;

    char *body_str = cJSON_GetObjectItem(json, "body")->valuestring;
    body = std::string(body_str == nullptr ? "" : body_str);

    return true;

}

long Envelope::get_sequence_number()
{
    return sequence_number;
}

void Envelope::set_sequence_number(long sequence_number)
{
    this->sequence_number = sequence_number;
}

std::string Envelope::get_uuid()
{
    return uuid;
}

void Envelope::set_uuid(std::string uuid)
{
    this->uuid = uuid;
}

std::string Envelope::get_topic()
{
    return topic;
}

void Envelope::set_topic(std::string topic)
{
    this->topic = topic;
}

long Envelope::get_timestamp()
{
    return timestamp;
}

void Envelope::set_timestamp(long timestamp)
{
    this->timestamp = timestamp;
}

std::string Envelope::get_body()
{
    return body;
}

void Envelope::set_body(std::string body)
{
    this->body = body;
}

std::string Envelope::get_error()
{
    return error;
}

void Envelope::set_error(std::string error)
{
    this->error = error;
}

bool Envelope::get_success()
{
    return success;
}

void Envelope::set_success(bool success)
{
    this->success = success;
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

    cJSON_AddItemToObject(recovery_request, "startSequenceNumber", startseqnum_json);
    cJSON_AddItemToObject(recovery_request, "endSequenceNumber", endseqnum_json);

    return cJSON_Print(recovery_request);

}

bool RecoveryRequest::Deserialize(std::string json_string)
{

    cJSON *json = cJSON_Parse(json_string.c_str());
    
    start_sequence_number = cJSON_GetObjectItemCaseSensitive(json, "startSequenceNumber")->valueint;
    end_sequence_number = cJSON_GetObjectItemCaseSensitive(json, "endSequenceNumber")->valueint;

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
    cJSON *array = cJSON_AddArrayToObject(result, "recoveryList");

    std::list<Envelope>::iterator it;
    for (it = message_envelopes.begin(); it != message_envelopes.end(); it++)
    {
        cJSON *envelope = it->SerializeToCJSON();
        
        cJSON_AddItemToArray(array, envelope);
    }
    return cJSON_Print(result);

}

bool RecoveryResponse::Deserialize(std::string json_string)
{
    cJSON *json = cJSON_Parse(json_string.c_str());
    if (cJSON_IsNull(json)) return false;

    cJSON *message_envelopes_array_json = cJSON_GetObjectItem(json, "recoveryList");
    if (cJSON_IsNull(message_envelopes_array_json)) return false;
    
    long size = cJSON_GetArraySize(message_envelopes_array_json);
    
    Envelope msg_envelope;

    for (long i = 0; i < size; i++)
    {
        cJSON *msg_envelope_json = cJSON_GetArrayItem(message_envelopes_array_json, i);
        msg_envelope.Deserialize(cJSON_Print(msg_envelope_json));

        message_envelopes.push_back(msg_envelope);
    }
    
    
    return true;

}

void RecoveryResponse::set_message_envelopes(std::list<Envelope> message_envelopes)
{
    this->message_envelopes = message_envelopes;
}

std::list<Envelope> RecoveryResponse::get_message_envelopes()
{
    return message_envelopes;
}
