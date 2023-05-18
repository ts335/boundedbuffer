#include <ledger.h>
#include <buffer.h>
using namespace std;

pthread_mutex_t ledger_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counterlock = PTHREAD_MUTEX_INITIALIZER; //lock for counter

list<struct Ledger> ledger;
Bank *bank;
Buffer *buffer; //buffer pointer 
int counter = 0; //initializing
int max_items = 0; //initializing
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
	bank = new Bank(10);
	buffer = new Buffer(8); //creating buffer object w/size

	bank->print_account();
	load_ledger(filename);
	max_items = ledger.size();
	int worker_ids[num_workers];
	int reader_ids[num_workers];
	//creating reader threads 
	pthread_t reader_threads[num_workers];
	for (int i = 0; i < num_workers; i++) {
		reader_ids[i] = i;
		//cout << "Creating reader thread: " << i << endl;
		pthread_create(&reader_threads[i], NULL, reader, &(reader_ids[i]));
	}
	//creating worker threads
	pthread_t worker_threads[num_workers];
	for (int i = 0; i < num_workers; i++) {
		worker_ids[i] = i;
		//cout << "Creating worker thread: " << i << endl;
		pthread_create(&worker_threads[i], NULL, worker, &(worker_ids[i]));
	}
	for (int i = 0; i < num_workers; i++) {
		pthread_join(reader_threads[i], NULL); //joining reader threads
	}
	for (int i = 0; i < num_workers; i++) {
		pthread_join(worker_threads[i], NULL); //joining worker threads
	}
	bank->print_account();
}

/**
 * @brief Parse a ledger file and store each line into a list
 * 
 * @param filename 
 */
void load_ledger(char *filename){
	//unchanged
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
	pthread_mutex_lock(&ledger_lock); //locking to prevent multiple threads from accessing at once
	while (!ledger.empty()) { //while the ledger list is not empty
		struct Ledger item = ledger.front(); //set this to item from ledger list
		ledger.pop_front(); //remove that item from ledger list
		pthread_mutex_unlock(&ledger_lock); //unlock
		buffer->push(item); //adding item to bugger

		pthread_mutex_lock(&ledger_lock); //locking again 
	}
	pthread_mutex_unlock(&ledger_lock); //unlock 
	return NULL;
}

void* worker(void *workerID){
	int id = (*(int*)workerID); //casting to int and dereferencing 
	while (true) {
		if (counter >= max_items) {
            break; //all ledger items have been processed so we break
		}
		struct Ledger item = buffer->pop(); //removing item from buffer 
		pthread_mutex_lock(&counterlock);
        counter++; //incrementing counter as we process ledger objects
        pthread_mutex_unlock(&counterlock);
		if (item.ledgerID == -1)
            break; 
		//bank options below 
		if (item.mode == 0) {
			bank->deposit(id, item.ledgerID, item.from, item.amount);
		} else if (item.mode == 1) {
			bank->withdraw(id, item.ledgerID, item.from, item.amount);
		} else if (item.mode == 2) {
			bank->transfer(id, item.ledgerID, item.from, item.to, item.amount);
		} else if (item.mode == 3) {
            bank->checkbalance(id, item.ledgerID, item.from); //added bank option for checkbalance()
        }
	
	}
	return NULL;
}
