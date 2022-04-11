#pragma once

#include <list>
#include <thread>
#include <queue>
#include <mutex>
#include <pqxx/pqxx>

#include "datatypes.hpp"

using namespace pqxx;

class PersistentStorageInterface;
class TestSubscriberPersistenceStorage;

class PersistentStorageInterface
{

public:
    virtual std::list<Message> get_messages(long start, long end) = 0;
    virtual void store_message(Message message) = 0;
    virtual long load_sequence_number() = 0;

};


class EmbeddedPersistenceStorage : public PersistentStorageInterface
{

public:
    std::list<Message> get_messages(long start, long end);
    void store_message(Message message);
    long load_sequence_number();

};

class TestPersistenceStorage : public PersistentStorageInterface
{

public:
    TestPersistenceStorage();
    
    virtual std::list<Message> get_messages(long start, long end);
    virtual void store_message(Message message);
    virtual long load_sequence_number();

    void join();
    void detach();

private:
    std::queue<Message> store_message_queue;
    long sequence_number = 0;
    std::list<Message> message_storage;
    std::mutex g_mutex;

    void thread_fn();

    std::thread *store_thread;

};

class TestSubscriberPersistenceStorage : public PersistentStorageInterface
{

public:
    TestSubscriberPersistenceStorage();
    
    virtual std::list<Message> get_messages(long start, long end);
    virtual void store_message(Message message);
    virtual long load_sequence_number();

    std::list<Message> get_messages();

    void join();
    void detach();

private:
    std::queue<Message> store_message_queue;
    long sequence_number = 0;
    std::list<Message> message_storage;
    std::mutex g_mutex;

    void thread_fn();

    std::thread *store_thread;

};

class PostgreSqlPersistentStorage : public PersistentStorageInterface
{
public:
    PostgreSqlPersistentStorage
    (
        std::string dbname,
        std::string user,
        std::string password,
        std::string hostaddr,
        std::string port
    );

    virtual std::list<Message> get_messages(long start, long end);
    virtual void store_message(Message message);
    virtual long load_sequence_number();

    void join();
    void detach();

private:
    connection *C;
    std::string connection_string;
    std::queue<Message> store_message_queue;
    std::thread *store_thread;
    std::mutex g_mutex;

    long sequence_number = 0;

    void thread_fn();
    std::string create_insert_sql(Message message);
    std::string create_select_sql(long start, long end);

};