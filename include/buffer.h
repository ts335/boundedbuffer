#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdlib.h>
#include <fstream>
#include <string>
#include <sys/wait.h>   /* for wait() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <sys/mman.h>   /* for mmap() ) */
#include <semaphore.h>  /* for sem */
#include <assert.h>		/* for assert */
#include <iostream>     /* for cout */
#include <list>
#include <array>
#include <pthread.h> 

using namespace std;

class Buffer {
  private:
    int size;
    int current; //number of elements in buffer
    int first_empty_slot;
    int last_filled_slot;
    struct Ledger *boundedbuffer;
    pthread_mutex_t lock;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
    
  public:
    Buffer(int N);
    ~Buffer(); // destructor
    
    struct Ledger pop();
    struct Ledger push(struct Ledger value);
};

#endif