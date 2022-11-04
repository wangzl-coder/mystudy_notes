#ifndef ARRAY_QUEUE_H__
#define ARRAY_QUEUE_H__


struct queue_st{
    void **parray;
    int len;
    int que_head;
    int que_tail;
};

struct queue_st *queue_init(int len);

int queue_send(struct queue_st *, void *data);

void *queue_receive(struct queue_st *);

int queue_is_empty(struct queue_st *);

int queue_is_full(struct queue_st *);

void queue_release(struct queue_st *);



#endif
