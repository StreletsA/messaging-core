#pragma once

#include <list>
#include <thread>
#include <queue>
#include <mutex>

#include "datatypes.hpp"

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