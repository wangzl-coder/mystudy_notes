#ifndef LIST_STACK_H__
#define LIST_STACK_H__

#include "list.h"

#define lstack_by_node(node)    \

typedef struct lstack_node_st{
    void *data;
    struct node_head node;
}stack_node;

typedef struct lstack_st{
    struct node_head head_node;
}lstack_t;

lstack_t *list_stack_init(int size);

int list_stack_push(lstack_t *, void *);

void *list_stack_pop(lstack_t *);

int list_stack_isempty(lstack_t *);

int list_stack_release(lstack_t *);

#endif
