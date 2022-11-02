#ifndef LIST_STACK_H__
#define LIST_STACK_H__

#include "list.h"

#define ITEM_OFFSET(type, item)     (size_t)(&(((type*)0)->item))

#define lstack_by_node(node)    ((stack_node*)(((char*)node) - ITEM_OFFSET(stack_node, node)))

typedef struct lstack_node_st{
    void *data;
    struct node_head node;
}stack_node;

typedef struct lstack_st{
    struct node_head head_node;
}lstack_t;

lstack_t *list_stack_init();

int list_stack_push(lstack_t *, void *);

void *list_stack_pop(lstack_t *);

int list_stack_isempty(lstack_t *);

void list_stack_release(lstack_t *);

#endif
