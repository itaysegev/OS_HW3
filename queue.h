#ifndef QUEUE_H
#define QUEUE_H

#include "segel.h"

typedef struct node_t
{
    int data;
    struct timeval req_arrival;
    struct node_t* next;
} node;

typedef struct queue_t
{
    int queue_size;
    node* head;
    node* tail;
} queue;

void enqueue(int connfd, struct timeval current_time, queue* queue);
node* dequeue(queue* queue);
//int removeFromQueue(Queue* queue, int index);
int deleteNode(queue* queue, int position);
queue* createQueue();
#endif