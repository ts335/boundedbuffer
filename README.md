# boundedbuffer
# Updates to Ledger.cpp
- void InitBank(int num_workers, char *filename)
This method initializes both the bank and buffer objects. It loads the ledger from a file, creates reader threads and writer threads, and joins both the reader and writer threads at the end. 

- void* reader(void *readerID)
 This function removes items from the ledger list, pushes them to the buffer, and repeats until the ledger list is empty. Thus we are now using the bounded buffer as the main structure that holds the ledger objects. "readerID" is a pointer to the reader thread id.

- void* worker(void *workerID)
This function processes items from the buffer, increments the counter, and performs corresponding operations on the bank object based on the ledger entry mode.

- void load_ledger(char *filename) 
This function has not been altered from original starter code. 

- Global Variables (Updates)
- "pthread_mutex_t counterlock" & "int counter": The counter variable and counterlock mutex are used to ensure thread safety when updating the counter variable by the worker threads. This keeps track of the items the worker threads have processed.
- "Buffer *buffer": Addeed pointer to buffer object
- "int max_items": total number of items in ledgere (max # of items that can be processed by buffer)
- All other variables are unaltered or were given in starter code.
# Updates to bank.cpp/bank.h
 - NOTE: Please see comments for breakdown of code logic (e.g. where reader/writer locks are located, initialized, destroyed, checkbalance() code logic, etc.)
- Added checkbalance() function that checks an accounts balance. Returns message in recordSucc in the proper format. Formatted similarly to the other functions in the file.
- Changed lock to reader/writer lock in bank.h. In bank.cpp The lock in checkbalance() has specifically been changed to a reader lock. While locks in deposit, withdraw, etc. were changed to writer locks since they are altering the amount in the account.
- Added checkbalance() as a bank operation in the worker function in ledger.cpp (will work depending on the ledger entry mode)
 # buffer.cpp and buffer.h (NEW class/header file)
 - NOTE: Please see comments for breakdown of code logic
 - This class creates a bounded buffer and implements pop() and push() functions to properly add or remove items to the buffer. The buffer allows multiple threads to push and pop ledger entries safely.
- Buffer(int N) is the constructor for the buffer class. It initializes the member variables, allocates memory for the bounded buffer, and initializes the mutex and condition variables.
- ~Buffer() is the deconstructor for the buffer class. It frees the memory allocated for the bounded buffer and destroys the mutex and condition variables.
- struct Ledger pop() is the function that removes ledger objects from the buffer. It works by locking the mutex, makes the threads wait if the buffer is empty, retrieves the last filled slot value, updates the indices and counters, signals the buffer is not full, and then unlocks the mutex.
- struct Ledger push(struct Ledger value) is the function that adds ledger objects to the buffer. It works by locking the mutex, makes the threads wait if the buffer is full, adds the entry to the first empty slot, updates the indices and counters, signals the buffer is not empty, and unlocks the mutex.

# Videos/Links
Presentation Video:
https://youtu.be/Ve5Q78FE_y0

Demo Video: 
https://youtu.be/1gs_aOgsyfY

Presentation overview slides:
https://docs.google.com/presentation/d/1LRjkfJbR1oGPv3eWieKRycQEEPDGA6RASEErnu2buso/edit?usp=sharing
