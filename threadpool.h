#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "thread.h"
#include "queue.h"

typedef struct thread_pool_t {
    thread* threads; //array of thread
    Queue* waiting_tasks_queue;
    int num_of_threads;
    int current_task_counter;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPoll;

ThreadPoll* create_new_pool(int num_of_threads);
void* thread_handle_request(void* args);

#endif