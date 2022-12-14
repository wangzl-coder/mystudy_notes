#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define ITEM_OFFSET(type, itname)  (size_t)(&(((type*)0)->itname))

#define ITEM_ENTRY(type, pitem, itname) ((type*) (((char *) pitem) - ITEM_OFFSET(type, itname)))

struct list_st{
    struct list_st *next;
    struct list_st *prev;
}list;

struct product_item_st{
    char *name;
    int num;
};

struct queue_st{
    list qhead;
    list *qtail;
};

typedef struct queue_item_st{
    list node;
    void *data;
}queue_item;

static int queue_init(struct queue_st *queue)
{
    if(queue == NULL)
        return -EINVAL;
    queue->qhead.next = NULL;
    queue->qhead.prev = NULL;
    queue->qtail = &queue->qhead;
    return 0;
}

/* add tail */
static int queue_in(struct queue_st *queue, void *data, unsigned int size)
{
    queue_item *item;
    if(queue == NULL)
        rerturn -EINVAL;
    if( (item = malloc(sizeof(queue_item))) == NULL)
        return -ENOMEM;
    if((newdata = malloc(size)) == NULL)
        goto item_free;
    memcpy(item->data, data, size);
    item->node.next = NULL;
    if(queue->qtail == &queue->head) {      /* null node yet */
        queue->head.next = &item->node;
        item->node.prev = &queue->head;
    } else {
        queue->qtail->next = &item->node;
        item->node.prev = queue->tail;
    }
    queue->qtail = &item->node;
    return 0;

item_free:
    free(item);
    return -ENOMEM;
}

/* del head */
static void *queue_out(struct queue_st *queue)
{
    list *del_node = queue->qtail;
    queue_item *del_item;
    if(del_node == &queue->head)    /* null node yet */
        return NULL;
    
    del_item = ITEM_ENTRY(queue_item, del_node, node);
    queue->qtail = del_node->prev;
    queue->qtail->next = NULL;
    free();
    free(del_item);
}

static void *producer(void *arg)
{
    struct product_st *product = arg;
          
    
}

static void *customer(void *arg)
{


}

int main()
{
    

    exit(0);
}
