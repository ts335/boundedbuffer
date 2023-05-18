#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <ledger.h>
#include <buffer.h>

Buffer::Buffer(int N) {
    boundedbuffer = (struct Ledger*)malloc(N * sizeof(struct Ledger));
    current = 0;
    size = N;
    first_empty_slot = 0;
    last_filled_slot = 0;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&notFull, NULL);
    pthread_cond_init(&notEmpty, NULL);
}

Buffer::~Buffer() { 
  free(boundedbuffer);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&notFull);
  pthread_cond_destroy(&notEmpty);
}

struct Ledger Buffer::pop() { //removes from buffer
    struct Ledger value;
    pthread_mutex_lock(&lock);
    while (current == 0) 
    //make threads wait if the buffer is empty
        pthread_cond_wait(&notEmpty, &lock); 
    value = boundedbuffer[last_filled_slot]; //setting value to the last filled slot
    last_filled_slot = (last_filled_slot + 1) % size;
    current--; //decrement since we are removing
    pthread_cond_signal(&notFull); //signaling buffer is not full - will wake threads if any are asleep
    pthread_mutex_unlock(&lock);
    return value;
}

struct Ledger Buffer::push(struct Ledger value) { //adds to buffer
    pthread_mutex_lock(&lock);
    while (current == size) 
    //make threads wait if the buffer is full
        pthread_cond_wait(&notFull, &lock);
    boundedbuffer[first_empty_slot] = value; //setting the empty slot to the desired value
    first_empty_slot = (first_empty_slot + 1) % size;
    current++; //increment since we are adding
    pthread_cond_signal(&notEmpty); //signaling buffer is not empty - will wake threads if any are asleep 
    pthread_mutex_unlock(&lock);
    return value;
}