#pragma once

#include <string>
#include <iostream>
#include <list>

#include "tools/cJSON.h"
#include "tools/uuid.hpp"

/*

    We should choose the type of message by topic name

*/

enum MessageType
{
    INFO,
    SUCCESS,
    REJECT,
    FAIL,
    QUOTE,
    REPLY
};

class Message
{

public:
    Message();
    Message(std::string data_json);
    Message
    (
        long sequence_number,
        std::string uuid,
        std::string topic,
        long timestamp,
        MessageType message_type,
        bool needs_reply,
        std::string data_json
    );

    std::string Serialize();
    cJSON* SerializeToCJSON();
    bool Deserialize(std::string s);

    long get_sequence_number();
    void set_sequence_number(long sequence_number);

    std::string get_topic();
    void set_topic(std::string topic);

    long get_timestamp();
    void set_timestamp(long timestamp);

    MessageType get_message_type();
    void set_message_type(MessageType message_type);

    std::string get_uuid();
    void set_uuid(std::string uuid);

    bool get_needs_reply();
    void set_needs_reply(bool needs_reply);

    std::string get_data();
    void set_data(std::string data_json);

private:
    long sequence_number;
    std::string uuid;
    std::string topic;
    long timestamp;
    MessageType message_type;
    bool needs_reply;
    std::string data_json;

};

class StringData
{

public:
    StringData();
    StringData(std::string str_data);

    std::string Serialize();
    bool Deserialize(std::string json_string);

    std::string get_str_data();
    void set_str_data(std::string str_data);

private:
    std::string str_data;

};

class RecoveryRequest
{

public:
    std::string Serialize();
    bool Deserialize(std::string json_string);

    void set_start_sequence_number(long start_sequence_number);
    void set_end_sequence_number(long end_sequence_number);

    long get_start_sequence_number();
    long get_end_sequence_number();

private:
    long start_sequence_number;
    long end_sequence_number;

};

class RecoveryResponse
{

public:
    std::string Serialize();
    bool Deserialize(std::string json_string);

    void set_messages(std::list<Message> messages);

    std::list<Message> get_messages();

private:
    std::list<Message> messages;

};