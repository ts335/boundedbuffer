# boundedbuffer
# Updates to Ledger.cpp
# Functions
- void InitBank(int num_workers, char *filename)
Initializes bank and buffer object
Creates Reader threads and writer threads and joins both threads at the end

- void* reader(void *readerID)
 This function removes items from the ledger list, pushes them to the buffer, and repeats until the ledger list is empty.

- void* worker(void *workerID)
This function processes items from the buffer, increments the counter, and performs corresponding operations on the bank object based on the ledger entry mode.

# Global Variables (Updates)
The counter variable and counterlock mutex are used to ensure thread safety when updating the counter variable by the worker threads.
# Updates to bank.cpp/bank.h
- Added checkbalance() function that cheecks an accounts balance
- 

# Videos
Presentation Video:
https://youtu.be/Ve5Q78FE_y0

Demo Video: 
https://youtu.be/1gs_aOgsyfY