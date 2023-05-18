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
    int size; //size of buffer
    int current; //number of elements in buffer
    int first_empty_slot; //index of first available empty slot
    int last_filled_slot; //index of last filled slot
    struct Ledger *boundedbuffer;
    pthread_mutex_t lock; //declaring lock
    pthread_cond_t notFull; //conditional for when buffer is not full
    pthread_cond_t notEmpty; //conditional for when buffer is not empty
    
  public:
    Buffer(int N);
    ~Buffer(); // destructor
    
    struct Ledger pop(); //removes item from buffer
    struct Ledger push(struct Ledger value); //adds item to buffer
};

#endif