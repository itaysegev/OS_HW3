#include "segel.h"
#include "request.h"
#include "threadpool.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
// void getargs(int *port, int argc, char *argv[])
// {
//     if (argc < 2) {
// 	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
// 	exit(1);
//     }
//     *port = atoi(argv[1]);
// }
void getargs(int *port, int *number_of_threads,int *max_requests, char **schedalg, int argc, char *argv[])
{
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *number_of_threads = atoi(argv[2]);
    *max_requests = atoi(argv[3]);
    *schedalg = argv[4];
}

int main(int argc, char *argv[])
{
    // int listenfd, connfd, port, clientlen;
    int listenfd, connfd, port, clientlen, number_of_threads, max_requests;
    struct sockaddr_in clientaddr;
    char* schedalg = NULL; 
    // getargs(&port, argc, argv);
    getargs(&port, &number_of_threads, &max_requests, &schedalg, argc, argv);
    ThreadPool* thread_pool = create_new_pool(number_of_threads);
    // 
    // HW3: Create some threads...
    //

    listenfd = Open_listenfd(port); 
	// // 
	// // HW3: In general, don't handle the request in the main thread.
	// // Save the relevant info in a buffer and have one of the worker threads 
	// // do the work. 
	// // 
    while (1) {
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen); // same as the segel's code
        struct timeval current_time;
        gettimeofday(&current_time, NULL);

	    if (strcmp(schedalg,"block") == 0) { // blocking policy
	        pthread_mutex_lock(&(thread_pool->mutex));
            // not enough buffers are available test 
            if(thread_pool->waiting_tasks_queue->queue_size + thread_pool->current_task_counter >= max_requests) {
                while (thread_pool->waiting_tasks_queue->queue_size + thread_pool->current_task_counter >= max_requests) {
                    pthread_cond_wait(&(thread_pool->cond), &(thread_pool->mutex)); //wait until buffer become available
                }
                enqueue(connfd, current_time, thread_pool->waiting_tasks_queue);//continue as usual
            }
            else {
                enqueue(connfd, current_time, thread_pool->waiting_tasks_queue); // if buffer is available continue as usual
            }
            pthread_cond_signal(&(thread_pool->cond)); // same as all policies 
            pthread_mutex_unlock(&(thread_pool->mutex));
	    }

	    else if(strcmp(schedalg,"dt") == 0) {
	        pthread_mutex_lock(&(thread_pool->mutex));
	        if(thread_pool->waiting_tasks_queue->queue_size + thread_pool->current_task_counter >= max_requests) {
                    Close(connfd); // drop the new request by closing the socket and continue listening
            }
	        else {
                enqueue(connfd, current_time, thread_pool->waiting_tasks_queue); // if buffer is available same as all policies 
	        }
            pthread_cond_signal(&(thread_pool->cond));
            pthread_mutex_unlock(&(thread_pool->mutex));
	    }

	    else if(strcmp(schedalg,"dh") == 0) {
            pthread_mutex_lock(&(thread_pool->mutex));
            if (thread_pool->waiting_tasks_queue->queue_size + thread_pool->current_task_counter >= max_requests) {
                Node *oldest_request = dequeue(thread_pool->waiting_tasks_queue); // queue for waiting requests only
                if (oldest_request == NULL) { // queue is empty so moving to dt policy
                    Close(connfd); 
                }
                else {
                    Close(oldest_request->data);// drop the oldest request
                    enqueue(connfd, current_time, thread_pool->waiting_tasks_queue); // add the new request 
                }
            }
            else {
                enqueue(connfd, current_time, thread_pool->waiting_tasks_queue);//same as all policies
            }
            pthread_cond_signal(&(thread_pool->cond));
            pthread_mutex_unlock(&(thread_pool->mutex));
        }

        else if (strcmp(schedalg, "random") == 0) {
            pthread_mutex_lock(&(thread_pool->mutex));
            if (thread_pool->waiting_tasks_queue->queue_size + thread_pool->current_task_counter >= max_requests) {
                dropRandomNodes(thread_pool->waiting_tasks_queue);
                enqueue(connfd, current_time, thread_pool->waiting_tasks_queue); // add the new request
            }
            else {
                enqueue(connfd, current_time, thread_pool->waiting_tasks_queue);
            }
            pthread_cond_signal(&(thread_pool->cond));
            pthread_mutex_unlock(&(thread_pool->mutex));
        }
    //     else if(strcmp(schedalg,"random") == 0) { 
    //         pthread_mutex_lock(&(thread_pool->mutex));
    //         if (thread_pool->waiting_tasks_queue->queue_size + thread_pool->current_task_counter >= max_requests) {
    //             if(thread_pool->waiting_tasks_queue->queue_size == 0) {
    //                 Close(connfd);
    //                 pthread_mutex_unlock(&(thread_pool->mutex));
    //                 continue;
    //             }
    //             int connsToDropCount = ceil((thread_pool->waiting_tasks_queue->queue_size)/2.0);
    //             printf("queue size before drop %d \n", thread_pool->waiting_tasks_queue->queue_size);
    //             for(int i=0; i < connsToDropCount; i++)
    //             {
    //                 int randomRequest = abs(rand()) % (thread_pool->waiting_tasks_queue->queue_size - 1);
    //                 int removed_connfd = deleteNode(thread_pool->waiting_tasks_queue, randomRequest);
    //                 Close(removed_connfd);
    //             }
    //             printf("queue size after drop %d \n", thread_pool->waiting_tasks_queue->queue_size);
    //         }
    //         enqueue(connfd, current_time, thread_pool->waiting_tasks_queue);
    //         pthread_cond_signal(&(thread_pool->cond));
    //         pthread_mutex_unlock(&(thread_pool->mutex));
    //     }
    // }
    }
}


    


 
