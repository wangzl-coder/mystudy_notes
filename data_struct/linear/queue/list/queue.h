#ifndef LIST_QUEUE_H__
#define LIST_QUEUE_H__

#include "list.h"

typedef struct queue_node_st{
    void *data;
    struct node_head node;
}queue_node;

typedef struct{
    struct node_head head_node;
}list_queue;


list_queue *queue_init();

int queue_send(list_queue *lqueue, void *data);

void *queue_receive(list_queue *lqueue);

int queue_is_empty(list_queue *lqueue);

void queue_release(list_queue *queue);

#endif
