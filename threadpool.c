#include "threadpool.h"
#include "segel.h"
#include "request.h"

typedef struct arg_struct
{
    ThreadPool* thread_pool;
    int thread_id;
} handle_func_args; // arguments for 


static thread* createThreadsArr(int num_of_threads, ThreadPool* thread_pool) {
    thread* new_arr = (thread*)malloc(num_of_threads * sizeof(thread)); 
    for(int i = 0 ; i < num_of_threads ; i++) {
        new_arr[i].thread_id = i;
        handle_func_args* args = (handle_func_args*) malloc(sizeof(handle_func_args));
        args->thread_pool = thread_pool;
        args->thread_id = i;
        pthread_create(&(thread_pool->threads[i].thread), NULL ,thread_handle_request, (void*)args);
    }
    return new_arr;
}

ThreadPool* create_new_pool(int num_of_threads) {
    ThreadPool* thread_pool = (ThreadPool*) malloc(sizeof(*thread_pool));
    thread_pool->num_of_threads = num_of_threads;
    thread_pool->waiting_tasks_queue = createQueue();
    thread_pool->threads = createThreadsArr(num_of_threads, thread_pool);
    pthread_mutex_init(&(thread_pool->mutex), NULL);
    pthread_cond_init(&(thread_pool->cond), NULL);
    return thread_pool;
}


void* thread_handle_request(void* args) {
    handle_func_args* curr_args = (handle_func_args*) args;
    ThreadPool* thread_pool = curr_args->thread_pool;
    int thread_id = curr_args->thread_id;
    while(1)
    {
        pthread_mutex_lock(&(thread_pool->mutex));
        while(thread_pool->waiting_tasks_queue->queue_size == 0) { // no tasks to perform
            pthread_cond_wait(&(thread_pool->cond), &(thread_pool->mutex)); 
        }
        Node* curr_task = dequeue(thread_pool->waiting_tasks_queue); // the queue is not empty
        thread_pool->current_task_counter++;
        pthread_mutex_unlock(&(thread_pool->mutex)); // from here is no longer critical code 
        struct timeval current_time; // see time.h for more info
        gettimeofday(&current_time, NULL); 
        current_time.tv_sec = difftime(current_time.tv_sec, curr_task->req_arrival.tv_sec);
        current_time.tv_usec = difftime(current_time.tv_usec, curr_task->req_arrival.tv_usec);
        struct timeval req_dispatch = current_time;
        //requestHandle(curr_task->value, curr_task->req_arrival, req_dispatch, thread_id, thread_pool);
        //this line is for advanced implementation, for request handle.
        requestHandle(curr_task->data); // request.h for more info
        Close(curr_task->data); // same as server.c 
        //update number of current tasks
        pthread_mutex_lock(&(thread_pool->mutex)); // critical code
        thread_pool->current_task_counter--;
        pthread_cond_signal(&thread_pool->cond); //same as in the tutorial
        pthread_mutex_unlock(&(thread_pool->mutex)); //end of critical code
        
    }
    return NULL;
}