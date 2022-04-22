#pragma once

#include <string>
#include <iostream>
#include <list>
#include <ctime>

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

class MessageTypeConverter
{

public:
    static MessageType fromString(std::string message_type)
    {
        if (message_type == "INFO") return MessageType::INFO;
        if (message_type == "SUCCESS") return MessageType::SUCCESS;
        if (message_type == "REJECT") return MessageType::REJECT;
        if (message_type == "FAIL") return MessageType::FAIL;
        if (message_type == "QUOTE") return MessageType::QUOTE;
        if (message_type == "REPLY") return MessageType::REPLY;

        return MessageType::INFO;
    }
    static std::string toString(MessageType message_type)
    {

        switch (message_type)
        {
            case MessageType::INFO:
                return "INFO";
                break;
            case MessageType::SUCCESS:
                return "SUCCESS";
                break;
            case MessageType::REJECT:
                return "REJECT";
                break;
            case MessageType::FAIL:
                return "FAIL";
                break;
            case MessageType::QUOTE:
                return "QUOTE";
                break;
            case MessageType::REPLY:
                return "REPLY";
                break;
        }

        return "";
    }
};

class Envelope
{

public:
    Envelope();
    Envelope(std::string data_json);
    Envelope
    (
        std::string topic,
        std::string uuid,
        long sequence_number,
        long timestamp,
        bool success,
        std::string error,
        std::string body
    );
    Envelope
    (
        std::string topic,
        std::string uuid,
        long timestamp,
        bool success,
        std::string error,
        std::string body
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

    std::string get_uuid();
    void set_uuid(std::string uuid);

    std::string get_body();
    void set_body(std::string body);

    std::string get_error();
    void set_error(std::string error);

    bool get_success();
    void set_success(bool success);

private:
    std::string topic;
    std::string uuid;

    long sequence_number;
    long timestamp;

    bool success;
    std::string error;

    std::string body;

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

    void set_message_envelopes(std::list<Envelope> messages);

    std::list<Envelope> get_message_envelopes();

private:
    std::list<Envelope> message_envelopes;

};