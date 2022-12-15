#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

#define ITEM_OFFSET(type, itname)  (size_t)(&(((type*)0)->itname))
#define ITEM_ENTRY(type, pitem, itname) ((type*) (((char *) pitem) - ITEM_OFFSET(type, itname)))

#define PRODUCT_MAX     (20)
#define PRODUCTOR_NUM   (3)
#define CUSTOMER_NUM    (10)

typedef struct list_st{
    struct list_st *next;
    struct list_st *prev;
}list;

struct queue_st{
    list qhead;
    size_t size;
};

typedef struct queue_item_st{
    list node;
    void *data;
}queue_item;

struct product_param_st{
    struct queue_st *queue;
    sem_t profull;
    sem_t proempty;
    pthread_mutex_t lock;
};

struct queue_st *queue_init(size_t datalen)
{
    struct queue_st *queue;
    queue = malloc(sizeof(struct queue_st));
    if(queue == NULL)
        return NULL;
    queue->qhead.next = &queue->qhead;
    queue->qhead.prev = &queue->qhead;
    queue->size = datalen;
    return queue;
}

/* add tail */
int queue_in(struct queue_st *queue, void *data)
{
    queue_item *item;
    if(queue == NULL)
        return -EINVAL;
    if( (item = malloc(sizeof(queue_item))) == NULL)
        return -ENOMEM;
    if((item->data = malloc(queue->size)) == NULL)
        goto item_free;
    memcpy(item->data, data, queue->size);
    item->node.next = &queue->qhead;
    item->node.prev = queue->qhead.prev;
    queue->qhead.prev->next = &item->node;
    queue->qhead.prev = &item->node;
    return 0;

item_free:
    free(item);
    return -ENOMEM;
}

/* del head */
int queue_out(struct queue_st *queue, void *data)
{
    list *del_node = queue->qhead.next;
    queue_item *item;
    if(del_node == &queue->qhead)
        return 0;
    item = ITEM_ENTRY(queue_item, del_node, node);
    memcpy(data, item->data, queue->size);
    del_node->next->prev = del_node->prev;
    del_node->prev->next = del_node->next;
    free(item->data);
    free(item);
    return 0;
}

void queue_destroy(struct queue_st *queue)
{
    list *item_node, *tmp;
    queue_item *item;
    int i = 0;
    for(item_node = queue->qhead.next; item_node != &queue->qhead; item_node = tmp) {
        item = ITEM_ENTRY(queue_item, item_node, node);
        tmp = item_node->next;
        free(item->data);
        free(item);
    }
    free(queue);
}

static int senval;

void *productor_process(void *arg)
{
    struct product_param_st *param = arg;
    while(1) {
        sleep(1);
        sem_wait(&param->profull);
        pthread_mutex_lock(&param->lock);
        senval ++;
        printf("put value %d \r\n", senval);
        queue_in(param->queue, &senval);
        pthread_mutex_unlock(&param->lock);
        sem_post(&param->proempty);
    }
    return (void *)0;
}

void *customer_process(void *arg)
{
    struct product_param_st *param = arg;
    while(1) {
        int recval;
        sleep(5);
        sem_wait(&param->proempty);
        pthread_mutex_lock(&param->lock);
        queue_out(param->queue, &recval);
        printf("catch number %d \r\n", recval);
        pthread_mutex_unlock(&param->lock);
        sem_post(&param->profull);
    }
    return (void *)0;
}

int main()
{
    int i;
    int err;
    pthread_t ptid[PRODUCTOR_NUM], ctid[CUSTOMER_NUM];
    struct product_param_st param;
    if ((err = pthread_mutex_init(&param.lock, NULL))) {
        fprintf(stderr, "wanrnning : failed to init lock : %s \r\n", strerror(err));
        exit(-1);
    }
    pthread_mutex_init(&param.lock, NULL);
    sem_init(&param.profull, 0, PRODUCT_MAX);
    sem_init(&param.proempty, 0, 0);
    param.queue = queue_init(sizeof(int));
    for(i = 0; i < PRODUCTOR_NUM; i++) {
        printf("pro: %d \r\n", i);
        pthread_create(&ptid[i], NULL, productor_process, &param);
    }
    for(i = 0; i < CUSTOMER_NUM; i++) {
        pthread_create(&ctid[i], NULL, customer_process, &param);
    }

    for(i = 0; i < PRODUCTOR_NUM; i++) {
        pthread_join(&ptid[i], NULL);
    }
    for(i = 0; i < CUSTOMER_NUM; i++) {
        pthread_join(&ctid[i], NULL);
    }
    pthread_mutex_destroy(&param.lock);
    sem_destroy(&param.profull);
    sem_destroy(&param.proempty);
    queue_destroy(param.queue);
    exit(0);
}
