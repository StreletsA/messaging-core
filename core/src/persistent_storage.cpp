#include "persistent_storage.hpp"

PersistentStorageInterface *PersistentStorage::getPersistentStorageInterface(std::string json_params)
{

	cJSON *json = cJSON_Parse(json_params.c_str());

	char* db_type = cJSON_GetObjectItemCaseSensitive(json, "databaseType")->valuestring;

	PersistentStorageInterface *persistentStorageInterface = NULL;
	
	if (strcmp(db_type, "POSTGRESQL") == 0)
	{
		persistentStorageInterface = new PostgreSqlPersistentStorage(json_params);
	}

	return persistentStorageInterface;

}

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

/*
    #########################################################
    
    PostgreSqlPersistentStorage

    #########################################################
*/

PostgreSqlPersistentStorage::PostgreSqlPersistentStorage
(
	std::string dbname,
    std::string user,
    std::string password,
    std::string hostaddr,
    std::string port
)
{

	connect(dbname, user, password, hostaddr, port);

}

PostgreSqlPersistentStorage::PostgreSqlPersistentStorage(std::string json_params)
{
	cJSON *json = cJSON_Parse(json_params.c_str());

	std::string dbname = cJSON_GetObjectItemCaseSensitive(json, "dbName")->valuestring;
	std::string user = cJSON_GetObjectItemCaseSensitive(json, "user")->valuestring;
	std::string password = cJSON_GetObjectItemCaseSensitive(json, "password")->valuestring;
	std::string hostaddr = cJSON_GetObjectItemCaseSensitive(json, "hostAddress")->valuestring;
	std::string port = cJSON_GetObjectItemCaseSensitive(json, "port")->valuestring;

	connect(dbname, user, password, hostaddr, port);
}

void PostgreSqlPersistentStorage::connect
(
	std::string dbname,
    std::string user,
    std::string password,
    std::string hostaddr,
    std::string port
)
{
	connection_string = \
	"dbname = " + dbname +\
	" user = " + user + \
	" password = " + password +\
	" hostaddr = " + hostaddr + \
	" port = " + port;

	try
	{
		C = new connection(connection_string);
		if (C->is_open())
		{
			std::cout << "PostgreSQL STORAGE: database opened successfully!" << '\n';
		}
		else
		{
			std::cout << "PostgreSQL STORAGE: can't open database!" << '\n';
			return;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return;
	}
	
	store_thread = new std::thread(&PostgreSqlPersistentStorage::thread_fn, this);
}

void PostgreSqlPersistentStorage::store_message(Message message)
{
	store_message_queue.push(message);
    sequence_number++;
}

std::string PostgreSqlPersistentStorage::create_insert_sql(Message message)
{

	std::string sequence_number_str = std::to_string(message.get_sequence_number());
	std::string uuid_str = message.get_uuid();
	std::string topic_str = message.get_topic();
	std::string timestamp_str = std::to_string(message.get_timestamp());
	std::string message_type_str = MessageTypeConverter::toString(message.get_message_type());
	std::string needs_reply_str = message.get_needs_reply() ? "true" : "false";
	std::string data_json_str = message.get_data();

	std::string sql = "INSERT INTO messages (sequence_number, uuid, topic, timestamp, message_type, needs_reply, data_json) " \
					"VALUES(" + sequence_number_str + ", " \
					"'" + uuid_str + "', " \
					"'" + topic_str + "', " \
					"" + timestamp_str + ", " \
					"'" + message_type_str + "', " \
					"" + needs_reply_str + ", " \
					"'" + data_json_str + "');";
	
	return sql;

}

std::string PostgreSqlPersistentStorage::create_select_sql(long start, long end)
{
	std::string start_str = std::to_string(start);
	std::string end_str = std::to_string(end);

	std::string sql = "SELECT * FROM messages WHERE sequence_number > " + start_str + " AND sequence_number < " + end_str;

	return sql;
}

std::list<Message> PostgreSqlPersistentStorage::get_messages(long start, long end)
{

	std::list<Message> messages;

	if (sequence_number == 0) return messages;
	
	try
	{
		std::string sql = create_select_sql(start, end);

		nontransaction N(*C);
		result R( N.exec( sql ));

		Message *msg;
		long sequence_number;
		std::string uuid;
		std::string topic;
		long timestamp;
		MessageType message_type;
		bool needs_reply;
		std::string data_json;

		for (result::const_iterator c = R.begin(); c != R.end(); ++c)
		{

			sequence_number = c[0].as<int>();
			uuid = c[1].as<std::string>();
			topic = c[2].as<std::string>();
			timestamp = c[3].as<int>();
			message_type = MessageTypeConverter::fromString(c[4].as<std::string>());
			needs_reply = c[5].as<bool>();
			data_json = c[6].as<std::string>();

			msg = new Message
			(
				sequence_number,
				uuid,
				topic,
				timestamp,
				message_type,
				needs_reply,
				data_json
			);

			messages.push_back(*msg);

		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return messages;

}

long PostgreSqlPersistentStorage::load_sequence_number()
{
	return sequence_number;
}

void PostgreSqlPersistentStorage::join()
{
	store_thread->join();
}

void PostgreSqlPersistentStorage::detach()
{
	store_thread->detach();
}

void PostgreSqlPersistentStorage::thread_fn()
{
	while (true)
	{
		while (!store_message_queue.empty())
		{
			
			try
			{
				Message message = store_message_queue.front();
				store_message_queue.pop();

				work W(*C);
				W.exec(create_insert_sql(message));
				W.commit();
			}
			catch (const std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
			

		}
	}
}
