#include <iostream>
#include <zmq.hpp>
#include <thread>
#include "publisher.hpp"
#include "subscriber.hpp"
#include "network_params.hpp"

/*
	gcc -c lmdb/mdb.c -o mdb.o
	gcc -c lmdb/midl.c -o midl.o
	g++ *.cpp tools/*.c tools/*.cpp -lzmq -lpthread -lpqxx -o core mdb.o midl.o

	./core

*/

void add_messages(Publisher *publisher)
{
	int i = 0;
	while(true)
	{

		i++;

		Message mes;
		mes.set_message_type(MessageType::INFO);
		mes.set_needs_reply(false);
		//mes.set_sequence_number(i);
		mes.set_timestamp(234241);
		mes.set_topic("TEST");
		mes.set_uuid(generate_uuid_v4());
		
		StringData mes_str_data("Good job!!!");

		mes.set_data(mes_str_data.Serialize());

		publisher->publish(mes);
	}
}

void add_message(Publisher *publisher)
{
	int i = 0;
	while(i < 20)
	{
		i++;

		Message mes;
		mes.set_message_type(MessageType::INFO);
		mes.set_needs_reply(false);
		mes.set_sequence_number(i);
		mes.set_timestamp(234241);
		mes.set_topic("TEST");
		mes.set_uuid(generate_uuid_v4());
		
		StringData mes_str_data("Good job!!!");

		mes.set_data(mes_str_data.Serialize());
		//std::cout << "Mes str -> " << mes_str_data.Serialize() << '\n';
		publisher->publish(mes.Serialize());

		//std::cout << "TEST PUBLISHED! -> " << mes.Serialize() << '\n';

		std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
}

void poll_message(Subscriber *subscriber)
{
	while(1)
	{
		std::string msg = subscriber->poll();
		if (msg != "")
		{
			std::cout << "POLLER: " << msg << '\n';
		}
	}
}


int main(int argc, char *argv[])
{
	std::cout << "Start!" << '\n';
	//zmq::context_t *ctx = new zmq::context_t(2);
	zmq::context_t *ctx = Context::getInstance().get_context();

	const char *addr_pub = "tcp://*:4533";
	const char *addr_rep = "tcp://*:9928";
	const char *addr_sub = "tcp://localhost:4533";
	const char *addr_req = "tcp://localhost:9928";

	std::string json_params = "{\"databaseType\": \"POSTGRESQL\", \"dbName\": \"messaging_core\", \"user\": \"postgres\", \"password\": \"postgres\", \"hostAddress\": \"127.0.0.1\", \"port\": \"5432\"}";

	//PostgreSqlPersistentStorage tps("messaging_core", "postgres", "postgres", "127.0.0.1", "5432");
	//PersistentStorageInterface *tps = PersistentStorage::getPersistentStorageInterface(json_params);
	PersistentStorageInterface *tps = new LmdbStorage();
	TestSubscriberPersistenceStorage tsps;

	if (&tps == nullptr) std::cout << "NULL!" << '\n';

	Publisher publisher(ctx, addr_pub, addr_rep, tps);
	Subscriber subscriber(ctx, &tsps, "TEST", addr_sub, addr_req);

	std::thread appender(add_messages, &publisher);
	std::thread poller(poll_message, &subscriber);
	
	publisher.join();
	subscriber.join();
	tps->join();
	tsps.join();
	appender.join();
	poller.join();

}