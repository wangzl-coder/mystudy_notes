#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue_pool.h"


struct queue_item_st{
    void *cont;
    struct queue_item_st *next_item;
    struct queue_item_st *prev_item;
};

struct queue_pool_st{
    int len;
    int count;
    int cpl_flag;
    pthread_mutex_t pool_lock;
    pthread_cond_t none_cond;
    pthread_cond_t full_cond;
    struct queue_item_st qhead;
    struct queue_item_st qtail;
};

queue_pool_t *queue_pool_init(int len)
{
    struct queue_pool_st *qpool_t;
    if((qpool_t = malloc(sizeof(struct queue_pool_st))) == NULL)
        return NULL;
    qpool_t->len = len;
    qpool_t->count = 0;
    if(pthread_mutex_init(&qpool_t->pool_lock, NULL)) {
        goto lock_failed;
    }
    if(pthread_cond_init(&qpool_t->none_cond, NULL)) {
        goto nonecond_failed;
    }
    if(pthread_cond_init(&qpool_t->full_cond, NULL)) {
        goto fullcond_failed;
    }
    qpool_t->cpl_flag = 0;
    qpool_t->qhead.next_item = &qpool_t->qtail;
    qpool_t->qhead.prev_item = NULL;
    qpool_t->qtail.prev_item = &qpool_t->qhead;
    qpool_t->qtail.next_item = NULL;
    return qpool_t;

fullcond_failed:
    pthread_cond_destroy(&qpool_t->none_cond);
nonecond_failed:
    pthread_mutex_destroy(&qpool_t->pool_lock);
lock_failed:
    free(qpool_t);
    return NULL;
}

int queue_pool_push(queue_pool_t *pool_t, void *data)
{
    struct queue_pool_st *qpool_t = pool_t;
    struct queue_item_st *qitem;
    qitem = malloc(sizeof(struct queue_item_st));
    if(qitem == NULL)
        return -ENOMEM;
    qitem->cont = data;
    pthread_mutex_lock(&qpool_t->pool_lock);
    while(qpool_t->count >= qpool_t->len) {
        pthread_cond_wait(&qpool_t->full_cond, &qpool_t->pool_lock);
    }
    qitem->prev_item = &qpool_t->qhead; /* insert head */
    qitem->next_item = qpool_t->qhead.next_item;
    qpool_t->qhead.next_item->prev_item = qitem;
    qpool_t->qhead.next_item = qitem;
    qpool_t->count ++;
    pthread_cond_signal(&qpool_t->none_cond);
    pthread_mutex_unlock(&qpool_t->pool_lock);
    return 0;
}

void *queue_pool_pull(queue_pool_t *pool_t)
{
    void *data;
    struct queue_pool_st *qpool_t = pool_t;
    struct queue_item_st *item;
    pthread_mutex_lock(&qpool_t->pool_lock);
    while(qpool_t->count <= 0 && (!qpool_t->cpl_flag)) {
        pthread_cond_wait(&qpool_t->none_cond, &qpool_t->pool_lock);
    }
    if(qpool_t->cpl_flag) {
        pthread_mutex_unlock(&qpool_t->pool_lock);
        return(NULL);
    }
    /* rm tail */
    item = qpool_t->qtail.prev_item;
    item->prev_item->next_item = &qpool_t->qtail;
    qpool_t->qtail.prev_item = item->prev_item;
    qpool_t->count --;
    data = item->cont;
    free(item);
    pthread_cond_signal(&qpool_t->full_cond);
    pthread_mutex_unlock(&qpool_t->pool_lock);
    return data;
}

void queue_pool_complete(queue_pool_t *pool_t)
{
    int cplflag = 0;
    struct queue_pool_st *qpool = pool_t;
    while(1) {
        pthread_mutex_lock(&qpool->pool_lock);
        if(qpool->count == 0) {
            cplflag = 1;
            qpool->cpl_flag = 1;
        }
        pthread_mutex_unlock(&qpool->pool_lock);
        usleep(2000);
        if(cplflag) {
            pthread_cond_broadcast(&qpool->none_cond);
            break;
        }
    }
}

void queue_pool_destroy(queue_pool_t *pool_t)
{
    struct queue_pool_st *qpool_t = pool_t;
    struct queue_item_st *item, *tmp;
    for(item = qpool_t->qhead.next_item; item != &qpool_t->qtail; ) {
        tmp = item->next_item;
        free(item);
        item = tmp;
    }
    pthread_mutex_destroy(&qpool_t->pool_lock);
    pthread_cond_destroy(&qpool_t->none_cond);
    pthread_cond_destroy(&qpool_t->full_cond);
    free(qpool_t);
}
