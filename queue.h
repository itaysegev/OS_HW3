#ifndef QUEUE_H
#define QUEUE_H

#include "segel.h"

typedef struct node_t
{
    int data;
    struct timeval req_arrival;
    struct node_t* next;
} Node;

typedef struct queue_t
{
    int queue_size;
    Node* head;
    Node* tail;
} Queue;

void enqueue(int connfd, struct timeval current_time, Queue* queue);
Node* dequeue(Queue* queue);
//int removeFromQueue(Queue* queue, int index);
int deleteNode(Queue* queue, int position);
queue* createQueue();
void dropRandomNodes(Queue* queue);
#endif