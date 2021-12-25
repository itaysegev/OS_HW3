#include "segel.h"
#include "queue.h"



queue* createQueue() {
    queue* new_queue = (queue*)malloc(sizeof(queue));
    new_queue ->head = NULL;
    new_queue ->tail = NULL;
    new_queue->queue_size =0;
    return new_queue
}

void enqueue(int connfd,struct timeval current_time, queue* queue)
{
    node* node = (node*)malloc(sizeof(node));
    node->data = connfd;
    node ->next = NULL;
    node->req_arrival = current_time;
    if(queue->head == NULL) // the queue is empty
    {
        queue->head = node;
        queue->tail = node;
    }
    else // enqueue to the end 
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->queue_size++;
}

node* dequeue(queue* queue)
{
    if(queue->queue_size == 0) { //queue is empty
        return NULL;
    }
    node* node = queue->head;
    node* new_head = queue->head->next;
    if(queue->head == queue ->tail) // only one node in queue
    {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else
    {
        queue->head = new_head;
    }
    queue->queue_size--;
    return node;
}

int deleteNode(queue* queue, int position)
{
    node* temp = queue->head;
    if (position == 0)
    {
        return dequeue(queue)->data;
    }
    for (int i=0; temp!=NULL && i<position-1; i++) // iterate until gets to the right position
        temp = temp->next;
    int data = temp->next->data;
    node* new_next = temp->next->next; //update the queue
    temp->next = new_next;
    queue->queue_size--;
    return data;
}