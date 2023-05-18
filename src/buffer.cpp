#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <ledger.h>
#include <buffer.h>

Buffer::Buffer(int N) {
    boundedbuffer = (struct Ledger*)malloc(N * sizeof(struct Ledger)); //initializing buffer
    current = 0; //current items in buffer initialized to 0
    size = N; //size is N whatever the user declared the program as
    first_empty_slot = 0; //initialized to 0
    last_filled_slot = 0; //initialized to 0
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&notFull, NULL);
    pthread_cond_init(&notEmpty, NULL);
}

Buffer::~Buffer() { 
  free(boundedbuffer); 
  pthread_mutex_destroy(&lock); //destroying lock
  pthread_cond_destroy(&notFull); 
  pthread_cond_destroy(&notEmpty);
}

struct Ledger Buffer::pop() { //removes from buffer
    struct Ledger value;
    pthread_mutex_lock(&lock); //locking to prevent multiple threads from accessing at once
    while (current == 0) //if buffer is empty
    //make threads wait if the buffer is empty
        pthread_cond_wait(&notEmpty, &lock); 
    value = boundedbuffer[last_filled_slot]; //setting value to the last filled slot
    last_filled_slot = (last_filled_slot + 1) % size;
    current--; //decrement since we are removing
    pthread_cond_signal(&notFull); //signaling buffer is not full - will wake threads if any are asleep
    pthread_mutex_unlock(&lock); //freeing lock
    return value; //returning item we just removed
}

struct Ledger Buffer::push(struct Ledger value) { //adds to buffer
    pthread_mutex_lock(&lock); //locking to prevent multiple threads from accessing at once
    while (current == size) //if buffer is full
    //make threads wait if the buffer is full
        pthread_cond_wait(&notFull, &lock);
    boundedbuffer[first_empty_slot] = value; //setting the empty slot to the desired value
    first_empty_slot = (first_empty_slot + 1) % size;
    current++; //increment since we are adding
    pthread_cond_signal(&notEmpty); //signaling buffer is not empty - will wake threads if any are asleep 
    pthread_mutex_unlock(&lock); //freeing lock
    return value;
}