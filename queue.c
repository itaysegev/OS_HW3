#include "segel.h"
#include "queue.h"
#include <math.h>




Queue* createQueue() {
    Queue* new_queue = (Queue*)malloc(sizeof(Queue));
    new_queue ->head = NULL;
    new_queue ->tail = NULL;
    new_queue->queue_size =0;
    return new_queue;
}

void enqueue(int connfd,struct timeval current_time, Queue* queue)
{
    Node* node = (Node*)malloc(sizeof(Node));
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

Node* dequeue(Queue* queue)
{
    if(queue->queue_size == 0) { //queue is empty
        return NULL;
    }
    Node* node = queue->head;
    Node* new_head = queue->head->next;
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

int deleteNode(Queue* queue, int position)
{
    Node* temp = queue->head;
    if (position == 0)
    {
        return dequeue(queue)->data;
    }
    for (int i=0; temp!=NULL && i<position-1; i++) // iterate until gets to the right position
        temp = temp->next;
    int data = temp->next->data;
    Node* new_next = temp->next->next; //update the queue
    temp->next = new_next;
    queue->queue_size--;
    return data;
}

static Node* ithNodeInQueue(Queue* queue, int i)
{
    if (queue->head == NULL || i >= queue->queue_size || i < 0)
        return NULL;

    Node* it = queue->head;
    for (int j = 0; j < i ; j++)
    {
        it = it->next;
    }

    return it;
}

void dropRandomNodes(Queue* queue)
{
    int drop_amount = ceil(queue->queue_size * 0.5);

    Node* node_to_drop = NULL;
    Node* pre_node_to_drop = NULL;
    while (drop_amount > 0)
    {
        int i = rand() % (queue->queue_size); // rand() generates number between 0 to RAND_MAX 
        node_to_drop = ithNodeInQueue(queue, i); // should not fail basically
        pre_node_to_drop = ithNodeInQueue(queue, i - 1);
        
        Close(node_to_drop->data);
        if (pre_node_to_drop != NULL)
        {
            pre_node_to_drop->next = node_to_drop->next;
        }
        else // in case node_to_drop is head
        {
            pre_node_to_drop = node_to_drop->next;
            queue->head= pre_node_to_drop;
        }

        if (node_to_drop == queue->tail)
        {
            queue->tail = pre_node_to_drop; // if node_to_drop was head as well, then tail == NULL
        }
        node_to_drop->next = NULL;
        free(node_to_drop);
        queue->queue_size--;
        drop_amount--;
    }
}