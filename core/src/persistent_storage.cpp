#include "persistent_storage.hpp"

TestPersistenceStorage::TestPersistenceStorage()
{
	store_thread = new std::thread(&TestPersistenceStorage::thread_fn, this);
}

std::list<Message> TestPersistenceStorage::get_messages(long start, long end)
{

	//std::cout << "STORAGE: STORAGE SIZE: " << message_storage.size() << " START: " << start << " END: " << end << '\n';

    std::list<Message> messages;
	std::list<Message>::iterator it;

	// Start-up (Synchronization)
	if (end == 0)
	{

		if (start < sequence_number)
		{
			int i = start;
			while(i < message_storage.size())
			{
				it = message_storage.begin();
				std::advance(it, i);
				messages.push_back(*it);
				i++;
			}

			return messages;
		}

		return messages;
	}

    int i = start;
	while(i < end)
	{
		it = message_storage.begin();
		std::advance(it, i);
		messages.push_back(*it);
		i++;
	}

    return messages;

}

void TestPersistenceStorage::store_message(Message message)
{

	store_message_queue.push(message);
    sequence_number++;

}

long TestPersistenceStorage::load_sequence_number()
{

}

void TestPersistenceStorage::thread_fn()
{

	while(true)
	{

		while(!store_message_queue.empty())
		{
			
			Message mes = store_message_queue.front();
			store_message_queue.pop();

			message_storage.push_back(mes);

		}

	}

}

void TestPersistenceStorage::join()
{
	store_thread->join();
}

void TestPersistenceStorage::detach()
{
	store_thread->detach();
}


/*
    #########################################################
    
    TestSubscriberPersistenceStorage

    #########################################################
*/

TestSubscriberPersistenceStorage::TestSubscriberPersistenceStorage()
{
	store_thread = new std::thread(&TestSubscriberPersistenceStorage::thread_fn, this);
}

std::list<Message> TestSubscriberPersistenceStorage::get_messages(long start, long end)
{

	//std::cout << "SUBSCRIBER STORAGE: STORAGE SIZE: " << message_storage.size() << " START: " << start << " END: " << end << '\n';

    std::list<Message> messages;
	std::list<Message>::iterator it;

	g_mutex.lock();
    int i = start;
	while(i < end)
	{
		it = message_storage.begin();
		std::advance(it, i);
		messages.push_back(*it);
		i++;
	}
	g_mutex.unlock();

    return messages;

}

std::list<Message> TestSubscriberPersistenceStorage::get_messages()
{
	return get_messages(0, message_storage.size());
}

void TestSubscriberPersistenceStorage::store_message(Message message)
{
	store_message_queue.push(message);
    sequence_number++;

}

long TestSubscriberPersistenceStorage::load_sequence_number()
{
	return sequence_number;
}

void TestSubscriberPersistenceStorage::thread_fn()
{

	while(true)
	{

		while(!store_message_queue.empty())
		{

			Message mes = store_message_queue.front();
			store_message_queue.pop();

			message_storage.push_back(mes);

			//g_mutex.lock();
			//std::cout << "SUBSCRIBER STORAGE: MESSAGE STORED! SEQ NUM: " << sequence_number << '\n';
			//g_mutex.unlock();

		}

	}

}

void TestSubscriberPersistenceStorage::join()
{
	store_thread->join();
}

void TestSubscriberPersistenceStorage::detach()
{
	store_thread->detach();
}

