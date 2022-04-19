#include "persistent_storage.hpp"

PersistentStorageInterface *PersistentStorage::getPersistentStorageInterface(std::string json_params)
{

	std::cout << "PERSISTENT STORAGE: JSON_PARAMS -> " << json_params << '\n';

	cJSON *json = cJSON_Parse(json_params.c_str());

	char* db_type = cJSON_GetObjectItemCaseSensitive(json, "databaseType")->valuestring;

	PersistentStorageInterface *persistentStorageInterface = NULL;
	
	if (strcmp(db_type, "POSTGRESQL") == 0)
	{
		persistentStorageInterface = new PostgreSqlPersistentStorage(json_params);
	}

	if (strcmp(db_type, "LMDB") == 0)
	{
		persistentStorageInterface = new LmdbStorage();
	}

	if (strcmp(db_type, "NULL_DB") == 0)
	{
		persistentStorageInterface = new TestSubscriberPersistenceStorage();
	}

	return persistentStorageInterface;

}

TestPersistenceStorage::TestPersistenceStorage()
{
	store_thread = new std::thread(&TestPersistenceStorage::thread_fn, this);
}

std::list<Message> TestPersistenceStorage::get_messages(long start, long end)
{

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
long TestPersistenceStorage::get_sequence_number()
{
	return sequence_number;
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
    
    LmdbStorage

    #########################################################
*/

LmdbStorage::LmdbStorage()
{
	sequence_number = load_sequence_number();
	std::cout << "LMDB_STORAGE: SEQ_NUM -> " << sequence_number << '\n';
	store_thread = new std::thread(&LmdbStorage::thread_fn, this);

}

std::list<Message> LmdbStorage::get_messages(long start, long end)
{

	std::list<Message> messages;

	if (start == 0) start = 1;
	if (sequence_number == 0) return messages;

	// Start-up
	if (end == 0) end = sequence_number;

	end++;
	
	try
	{

		Message *msg;

		for (int i = start; i < end; i++)
		{
			std::string required_seq_num = std::to_string(i);

			char* key = (char *) required_seq_num.c_str();

			char* msg_str = getdb(key);

			if (msg_str == nullptr)
			{
				std::cout << "LMDB_STORAGE: GET MESSAGES: KEY NOT FOUND -> " << key << " (sequence number)" << '\n';
				continue;
			}

			msg = new Message();
			msg->Deserialize(msg_str);

			std::lock_guard<std::mutex> lg(g_mutex);
			messages.push_back(*msg);

		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return messages;

}

void LmdbStorage::store_message(Message message)
{
	sequence_number++;
	message.set_sequence_number(sequence_number);
	store_message_queue.push(message);

}

long LmdbStorage::load_sequence_number()
{
	int rc;
	MDB_env *env;
	MDB_stat stat;
	MDB_envinfo *info;
	MDB_dbi dbi;
    MDB_txn *txn;
	
	E(mdb_env_create(&env));

    /* Set the cache size */
    E(mdb_env_set_mapsize(env, CACHE_SIZE));
    E(mdb_env_open(env, DBDIR, 0, 0664));

    /* Get some data */
    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));

	mdb_dbi_close(env, dbi);
    mdb_env_close(env);

	mdb_stat(txn, dbi, &stat);

	return stat.ms_entries;
}

int LmdbStorage::putdb(char* key, char* value)
{
	
    int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val thekey, data, rdata;
    char sval[32];
	
    E(mdb_env_create(&env));

    /* Set the cache size */
    E(mdb_env_set_mapsize(env, CACHE_SIZE));
    E(mdb_env_open(env, DBDIR, MDB_CREATE, 0664));

    /* Put some data */
    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));

    thekey.mv_data = key;
    thekey.mv_size = strlen(key);
    data.mv_data = value;
    data.mv_size = strlen(value);

    E(mdb_put(txn, dbi, &thekey, &data, 0));

    E(mdb_txn_commit(txn));
	
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);

    return 0;

}

char* LmdbStorage::getdb(char* key)
{
	int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val thekey, data, rdata;

    //printf("get %s\n", thekey);

    E(mdb_env_create(&env));

    /* Set the cache size */
    E(mdb_env_set_mapsize(env, CACHE_SIZE));
    E(mdb_env_open(env, DBDIR, 0, 0664));

    /* Get some data */
    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));

    thekey.mv_data = key;
    thekey.mv_size = strlen(key);

    rc = mdb_get(txn, dbi, &thekey, &data);
    if (!rc) {
        char *valuen = (char *) malloc(data.mv_size + 1);
        memcpy(valuen, data.mv_data, data.mv_size);
        valuen[data.mv_size] = 0;
		
		E(mdb_txn_commit(txn));

		mdb_dbi_close(env, dbi);
		mdb_env_close(env);
		
        return valuen;
    } else {
        std::cout << "LMDB_STORAGE: GET FROM DB: NO SUCH KEY! -> " << key << '\n';
		return nullptr;
    }

    

    return nullptr;
}

long LmdbStorage::get_sequence_number()
{
	return sequence_number;
}
void LmdbStorage::join()
{
	store_thread->join();
}

void LmdbStorage::thread_fn()
{

	MDB_txn *transaction;

	while (true)
	{

		while (!store_message_queue.empty())
		{

			Message msg = store_message_queue.front();
			store_message_queue.pop();

			std::string msg_str = msg.Serialize();

			char* key = (char *) std::to_string(msg.get_sequence_number()).c_str();
			char* value = (char *) msg_str.c_str();

			if (putdb(key, value) != 0)
			{
				std::cout << "LMDB_STORAGE: STORE MESSAGE: FAIL!" << '\n';
			}

		}

	}
}

/*
    #########################################################
    
    TestSubscriberPersistenceStorage

    #########################################################
*/

TestSubscriberPersistenceStorage::TestSubscriberPersistenceStorage()
{
	store_thread = new std::thread(&TestSubscriberPersistenceStorage::thread_fn, this);
	sequence_number = 0;
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

long TestSubscriberPersistenceStorage::get_sequence_number()
{
	return sequence_number;
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
    std::string port,
	std::string table_name
)
{
	this->table_name = table_name;

	connect(dbname, user, password, hostaddr, port);

}

PostgreSqlPersistentStorage::PostgreSqlPersistentStorage(std::string json_params)
{

	std::cout << "POSTGRESQL: JSON_PARAMS -> " << json_params << '\n';

	cJSON *json = cJSON_Parse(json_params.c_str());

	std::string dbname = cJSON_GetObjectItemCaseSensitive(json, "dbName")->valuestring;
	std::string user = cJSON_GetObjectItemCaseSensitive(json, "user")->valuestring;
	std::string password = cJSON_GetObjectItemCaseSensitive(json, "password")->valuestring;
	std::string hostaddr = cJSON_GetObjectItemCaseSensitive(json, "hostAddress")->valuestring;
	std::string port = cJSON_GetObjectItemCaseSensitive(json, "port")->valuestring;
	
	table_name = cJSON_GetObjectItemCaseSensitive(json, "tableName")->valuestring;

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
		connection *C = new connection(connection_string);
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

	sequence_number = load_sequence_number();
	std::cout << "POSTGRESQL: SEQ_NUM -> " << sequence_number << '\n';
	
	store_thread = new std::thread(&PostgreSqlPersistentStorage::thread_fn, this);
}

void PostgreSqlPersistentStorage::store_message(Message message)
{
	std::lock_guard<std::mutex> lg(g_mutex);
	store_message_queue.push(message);
    sequence_number++;
}

long PostgreSqlPersistentStorage::get_sequence_number()
{
	return sequence_number;
}

std::string PostgreSqlPersistentStorage::create_insert_sql(Message message)
{

	//std::string sequence_number_str = std::to_string(message.get_sequence_number());
	std::string uuid_str = message.get_uuid();
	std::string topic_str = message.get_topic();
	std::string timestamp_str = std::to_string(message.get_timestamp());
	std::string body_json = message.get_body();

	/*
	std::string sql = "INSERT INTO " + table_name + " (sequence_number, uuid, topic, timestamp, body) " \
					"VALUES(" + sequence_number_str + ", " \
					"'" + uuid_str + "', " \
					"'" + topic_str + "', " \
					"" + timestamp_str + ", " \
					"'" + body_json + "');";
	*/

	std::string sql = "INSERT INTO " + table_name + "(uuid, topic, timestamp, body) " \
					"VALUES('" + uuid_str + "', " \
					"'" + topic_str + "', " \
					"" + timestamp_str + ", " \
					"'" + body_json + "');";
	
	return sql;

}

std::string PostgreSqlPersistentStorage::create_select_sql(long start, long end)
{
	std::string start_str = std::to_string(start);
	std::string end_str = std::to_string(end);

	std::string sql = "SELECT * FROM " + table_name + " WHERE sequence_number >= " + start_str + " AND sequence_number < " + end_str;

	return sql;
}

std::list<Message> PostgreSqlPersistentStorage::get_messages(long start, long end)
{

	std::list<Message> messages;

	if (sequence_number == 0) return messages;

	// Start-up
	if (end == 0) end = sequence_number;

	end++;
	
	try
	{
		std::string sql = create_select_sql(start, end);
		connection *C = new connection(connection_string);
		nontransaction N(*C);
		result R( N.exec( sql ));

		Message *msg;
		long sequence_number;
		std::string uuid;
		std::string topic;
		long timestamp;
		MessageType message_type;
		bool needs_reply;
		std::string body;

		for (result::const_iterator c = R.begin(); c != R.end(); ++c)
		{

			sequence_number = c[2].as<int>();
			uuid = c[1].as<std::string>();
			topic = c[0].as<std::string>();
			timestamp = c[3].as<int>();
			body = c[4].as<std::string>();

			msg = new Message
			(
				topic,
				uuid,
				sequence_number,
				timestamp,
				body
			);

			std::lock_guard<std::mutex> lg(g_mutex);
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
	std::string sql = "SELECT max(sequence_number) FROM " + table_name + ";";

	try{
		connection *C = new connection(connection_string);
		nontransaction N(*C);
		result R( N.exec( sql ));
		sequence_number = R.begin()[0].as<int>();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}


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
				std::lock_guard<std::mutex> lg(g_mutex);
				Message message = store_message_queue.front();
				store_message_queue.pop();

				connection *C = new connection(connection_string);
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
