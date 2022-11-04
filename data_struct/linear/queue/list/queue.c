#include <stdlib.h>
#include <errno.h>
#include "list.h"
#include "queue.h"

#define ITEM_OFFSET(type, item)     (size_t)(&(((type *) 0)->item))

#define queue_by_node(node) (queue_node *) (((char*) node) - ITEM_OFFSET(queue_node, node))

list_queue *queue_init()
{
    list_queue *lqueue = malloc(sizeof(list_queue));
    if(lqueue != NULL)
        INIT_LIST_HEAD(lqueue->head_node);
    return lqueue;
}

int queue_send(list_queue *lqueue, void *data)
{
    queue_node *new;
    if(lqueue == NULL)
        return -EINVAL;

    new = malloc(sizeof(queue_node));
    if(new == NULL)
        return -ENOMEM;
    new->data = data;
    return list_insert_head(&lqueue->head_node, &new->node);
}

void *queue_receive(list_queue *lqueue)
{
    void *data = NULL;
    struct node_head *node;
    queue_node *queue_del;
    
    if(lqueue == NULL)
        return NULL;

    node = list_delfetch_tail(&lqueue->head_node);
    if(node != NULL) {
        queue_del = queue_by_node(node);
        data = queue_del->data;
        free(queue_del);
    }
    return data;
}

int queue_is_empty(list_queue *lqueue)
{
    return list_is_empty(&lqueue->head_node);
}

void queue_release(list_queue *queue)
{
    struct node_head *node = NULL;
    queue_node *queue_del;
    if(queue == NULL)
        return;

    while((node = list_delfetch_head(&queue->head_node)) != NULL) {
        queue_del = queue_by_node(node);
        free(queue_del);
        node = NULL;
    }
    free(queue);
}
