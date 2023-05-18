#include <ledger.h>
#include <buffer.h>
using namespace std;

pthread_mutex_t ledger_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counterlock = PTHREAD_MUTEX_INITIALIZER;

list<struct Ledger> ledger;
Bank *bank;
Buffer *buffer;
int counter = 0;
int max_items = 0;
// 
/**
 * @brief creates a new bank object and sets up workers
 * 
 * Requirements:
 *  - Create a new Bank object class with 10 accounts.
 *  - Load the ledger into a list
 *  - Set up the worker threads.  
 *  
 * @param num_workers 
 * @param filename 
 */
void InitBank(int num_workers, char *filename) {
	//TODO: initialize the bank object with 10 accounts
	bank = new Bank(10);
	buffer = new Buffer(50);

	bank->print_account();
	//TODO: call load_ledger() to parse the file given by filename
	load_ledger(filename);
	max_items = ledger.size();
	cout << "SIZE" << max_items << endl;
	int worker_ids[num_workers];
	int reader_ids[num_workers];

	
	//have worker threads and loader threads - same data type but will have diff numbers
	pthread_t reader_threads[num_workers];
	for (int i = 0; i < num_workers; i++) {
		reader_ids[i] = i;
		//cout << "Creating reader thread: " << i << endl;
		pthread_create(&reader_threads[i], NULL, reader, &(reader_ids[i]));
	}

	pthread_t worker_threads[num_workers];
	for (int i = 0; i < num_workers; i++) {
		worker_ids[i] = i;
		//cout << "Creating worker thread: " << i << endl;
		pthread_create(&worker_threads[i], NULL, worker, &(worker_ids[i]));
	}
	for (int i = 0; i < num_workers; i++) {
		pthread_join(reader_threads[i], NULL);
	}
	for (int i = 0; i < num_workers; i++) {
		pthread_join(worker_threads[i], NULL);
	}
	bank->print_account();
}

/**
 * @brief Parse a ledger file and store each line into a list
 * 
 * @param filename 
 */
void load_ledger(char *filename){
	//here add multiple threads 
	//open up file outside of this function
	//and have threads go in to grab something from file they append to the list
	ifstream infile(filename);
	int f, t, a, m, ledgerID=0;
	while (infile >> f >> t >> a >> m) {
		struct Ledger l;
		l.from = f;
		l.to = t;
		l.amount = a;
		l.mode = m;
		l.ledgerID = ledgerID++;
		ledger.push_back(l);
	}
}

/**
 * @brief Remove items from the list and execute the instruction.
 * 
 * @param workerID 
 * @return void* 
 */

void* reader(void *readerID) {
	int id = (*(int*)readerID);
	cout << "Getting reader id: " << id << endl;
	pthread_mutex_lock(&ledger_lock);

	while (!ledger.empty()) {
		cout << "Beginning of reader loop - Reader id: " <<  id << endl;
		struct Ledger item = ledger.front();
		ledger.pop_front();
		pthread_mutex_unlock(&ledger_lock); 
		cout << "Item was added" << " Reader id: " <<  id << endl;
		buffer->push(item);
		cout << "Item was pushed to buffer - id: " << id << endl;
		cout << "Reader ID BEFORE counter: " << id << endl;
		cout << "Reader ID AFTER counter: " << id << endl;

		pthread_mutex_lock(&ledger_lock); 
	}
	cout << "BEFORE unlock - reader id: " << id << endl;
	pthread_mutex_unlock(&ledger_lock); 
	cout << "AFTER unlock - reader id:" << id << endl;
	return NULL;
}

void* worker(void *workerID){
	int id = (*(int*)workerID); //casting to int and dereferencing 
	//pthread_mutex_lock(&ledger_lock);
	cout << "Getting worker id: " << id << endl;
	while (true) {
		if (counter >= max_items) {
			cout << "Counter should break here. " << counter << id << endl;
            return NULL;
		}
		cout << "Beginning of worker loop - worker id: " << id << endl;
		//getting first item off of the list
		
		struct Ledger item = buffer->pop(); //struct Ledger item = ledger.front();
		cout << "BEFORE counter: " << counter << endl;
		pthread_mutex_lock(&counterlock);
		cout << "DURING counter: " << counter << endl;
        counter++;
        pthread_mutex_unlock(&counterlock);
		cout << "Got item off of buffer - worker id: " << id << endl;
		if (item.ledgerID == -1)
            break;
		//pthread_mutex_unlock(&ledger_lock); 
		if (item.mode == 0) {
			bank->deposit(id, item.ledgerID, item.from, item.amount);
		} else if (item.mode == 1) {
			bank->withdraw(id, item.ledgerID, item.from, item.amount);
		} else if (item.mode == 2) {
			bank->transfer(id, item.ledgerID, item.from, item.to, item.amount);
		} else if (item.mode == 3) {
            bank->checkbalance(id, item.ledgerID, item.from); //added
        }
		//pthread_mutex_lock(&ledger_lock);
		
       cout << "AFTER counter: " << counter << endl;
	}
	//pthread_mutex_unlock(&ledger_lock); 
	cout << "Counter should break here out in here. " << counter << endl;
	return NULL;
}
