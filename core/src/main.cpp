#include <iostream>
#include <zmq.hpp>
#include <thread>
#include "publisher.hpp"
#include "subscriber.hpp"

/*

	g++ main.cpp messaging-core/publisher.cpp messaging-core/subscriber.cpp messaging-core/tools/uuid.cpp messaging-core/tools/cJSON.c messaging-core/datatypes.cpp messaging-core/persistent_storage.cpp -lzmq -lpthread -o core
	
	./core

	CREATE LIB:
	g++ -fPIC main.cpp messaging-core/publisher.cpp messaging-core/subscriber.cpp messaging-core/tools/uuid.cpp messaging-core/tools/cJSON.c messaging-core/datatypes.cpp messaging-core/serializable.cpp messaging-core/persistent_storage.cpp -lzmq -lpthread -o messaging-core.o

	g++ messaging-core.o -shared -o lib-messaging-core.so

	/home/andrey/Development/Projects/Ammer/message-core-cpp/src/libmessaging-core.so

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
		mes.set_sequence_number(i);
		mes.set_timestamp(234241);
		mes.set_topic("TEST");
		mes.set_uuid("wewfeer");
		
		StringData mes_str_data("Good job!!!");

		mes.set_data(mes_str_data.Serialize());

		publisher->publish(mes);

		std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
}

void add_message(Publisher *publisher)
{
	int i = 0;
	while(1)
	{
		i++;

		Message mes;
		mes.set_message_type(MessageType::INFO);
		mes.set_needs_reply(false);
		mes.set_sequence_number(i);
		mes.set_timestamp(234241);
		mes.set_topic("TEST");
		mes.set_uuid("wewfeer");
		
		StringData mes_str_data("Good job!!!");

		mes.set_data(mes_str_data.Serialize());
		//std::cout << "Mes str -> " << mes_str_data.Serialize() << '\n';
		publisher->publish(mes);

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
	zmq::context_t *ctx = new zmq::context_t(2);

	const char *addr_pub = "tcp://*:4533";
	const char *addr_rep = "tcp://*:9928";
	const char *addr_sub = "tcp://localhost:4533";
	const char *addr_req = "tcp://localhost:9928";

	TestPersistenceStorage tps;
	TestSubscriberPersistenceStorage tsps;

	Publisher publisher(ctx, addr_pub, addr_rep, &tps);
	Subscriber subscriber(ctx, &tsps, "TEST", addr_sub, addr_req);

	//add_message(&publisher);

	//std::thread appender(add_messages, &publisher);
	//std::thread poller(poll_message, &subscriber);
	
	publisher.join();
	subscriber.join();
	tps.join();
	tsps.join();
	//appender.join();
	//poller.join();

}