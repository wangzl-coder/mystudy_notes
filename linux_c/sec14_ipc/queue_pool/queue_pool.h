#ifndef QUEUE_POOL_H__
#define QUEUE_POOL_H__

typedef void queue_pool_t;
            
queue_pool_t *queue_pool_init(int len);
int queue_pool_push(queue_pool_t *pool_t, void *data);
void *queue_pool_pull(queue_pool_t *pool_t);
void queue_pool_complete(queue_pool_t *pool_t);
void queue_pool_destroy(queue_pool_t *pool_t);

#endif
