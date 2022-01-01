#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef struct thread_t { 
    pthread_t thread;
    int thread_id;
    int stat_thread_count;
    int stat_thread_static;
    int stat_thread_dynamic;
} thread;
#endif