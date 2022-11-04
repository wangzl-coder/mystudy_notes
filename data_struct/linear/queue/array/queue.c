#include <stdlib.h>
#include <errno.h>
#include "queue.h"

struct queue_st *queue_init(int len)
{
    struct queue_st *queue = malloc(sizeof(struct queue_st));
    if(queue != NULL) {
        queue->len = len;
        queue->que_head = queue->que_tail = 0;
        queue->parray = malloc(len * sizeof(void *));
        if(queue->parray == NULL) {
            free(queue);
            queue = NULL;
        }
    }
    return queue;
}

int queue_send(struct queue_st *queue, void *data)
{
    int nextItem;
    if(queue == NULL || queue->parray == NULL)
        return -EINVAL;

    nextItem = (queue->que_tail + 1)%queue->len;
    if(nextItem == queue->que_head)
        return -ENOSPC;

    queue->parray[nextItem] = data;
    queue->que_tail = nextItem;
    return 0;
}

void *queue_receive(struct queue_st *queue)
{
    int nextItem = 0;
    void *data = NULL;
    if(queue == NULL)
        return NULL;

    if(queue->que_head == queue->que_tail)
        return NULL;
    
    queue->que_head = (queue->que_head + 1)%queue->len;
    return queue->parray[queue->que_head];

}

int queue_is_empty(struct queue_st *queue)
{
    return (queue->que_head == queue->que_tail);
}

int queue_is_full(struct queue_st *queue)
{
    return (((queue->que_tail +1)%queue->len) == queue->que_head);
}

void queue_release(struct queue_st *queue)
{
    if(queue == NULL)
        return ;

    free(queue->parray);
    free(queue);
}
