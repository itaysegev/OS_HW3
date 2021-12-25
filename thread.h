#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef struct thread_t { 
    pthread_t thread;
    int thread_id;
    /// additional data if needed 
} thread;
#endif