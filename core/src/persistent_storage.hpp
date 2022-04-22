#pragma once

#include <list>
#include <thread>
#include <queue>
#include <mutex>
#include <pqxx/pqxx>

#include "lmdb/lmdb.h"
#include "datatypes.hpp"

#define BUF_SIZE 1024
#define CACHE_SIZE 1UL * 1024UL * 1024UL * 1024UL

#define E(expr) CHECK((rc = (expr)) == MDB_SUCCESS, #expr)
#define CHECK(test, msg) ((test) ? (void)0 : ((void)fprintf(stderr, \
    "%s:%d: %s: %s\n", __FILE__, __LINE__, msg, mdb_strerror(rc)), abort()))
#define DBDIR "./messaging_core_db"

using namespace pqxx;

class PersistentStorageInterface;
class TestSubscriberPersistenceStorage;
class PostgreSqlPersistentStorage;

class PersistentStorage
{
public:
    static PersistentStorageInterface *getPersistentStorageInterface(std::string json_params);
};

class PersistentStorageInterface
{

public:
    virtual std::list<Envelope> get_message_envelopes(long start, long end) = 0;
    virtual void store_message_envelope(Envelope message_envelope) = 0;
    virtual long load_sequence_number() = 0;

    virtual long get_sequence_number() = 0;
    virtual void join() = 0;

};

class LmdbStorage : public PersistentStorageInterface
{

public:
    LmdbStorage();
    virtual std::list<Envelope> get_message_envelopes(long start, long end);
    virtual void store_message_envelope(Envelope message_envelope);
    virtual long load_sequence_number();

    virtual long get_sequence_number();
    virtual void join();

private:
    std::queue<Envelope> store_message_envelope_queue;
    long sequence_number;
    std::mutex g_mutex;
    
    int putdb(char *key, char *value);
    char* getdb(char *key);

    void thread_fn();

    std::thread *store_thread;
};

class TestPersistenceStorage : public PersistentStorageInterface
{

public:
    TestPersistenceStorage();
    
    virtual std::list<Envelope> get_message_envelopes(long start, long end);
    virtual void store_message_envelope(Envelope message_envelope);
    virtual long load_sequence_number();

    virtual long get_sequence_number();

    void join();
    void detach();

private:
    std::queue<Envelope> store_message_envelope_queue;
    long sequence_number = 0;
    std::list<Envelope> message_envelopes_storage;
    std::mutex g_mutex;

    void thread_fn();

    std::thread *store_thread;

};

class TestSubscriberPersistenceStorage : public PersistentStorageInterface
{

public:
    TestSubscriberPersistenceStorage();
    
    virtual std::list<Envelope> get_message_envelopes(long start, long end);
    virtual void store_message_envelope(Envelope message_envelope);
    virtual long load_sequence_number();
    virtual long get_sequence_number();
    std::list<Envelope> get_message_envelopes();

    void join();
    void detach();

private:
    std::queue<Envelope> store_message_envelope_queue;
    long sequence_number = 0;
    std::list<Envelope> message_envelopes_storage;
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
        std::string port,
        std::string table_name
    );
    PostgreSqlPersistentStorage(std::string json_params);

    virtual std::list<Envelope> get_message_envelopes(long start, long end);
    virtual void store_message_envelope(Envelope message_envelope);
    virtual long load_sequence_number();

    long load_sequence_number_for_topic(std::string topic);
    long get_sequence_number();

    void join();
    void detach();

private:
    std::string table_name;
    std::string connection_string;
    std::queue<Envelope> store_message_envelope_queue;
    std::thread *store_thread;
    std::mutex g_mutex;

    long sequence_number = 0;

    void connect
    (
        std::string dbname,
        std::string user,
        std::string password,
        std::string hostaddr,
        std::string port
    );
    void thread_fn();

    std::string create_insert_sql(Envelope message_envelope);
    std::string create_select_sql(long start, long end);

};