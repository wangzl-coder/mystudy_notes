#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "stack.h"

lstack_t *list_stack_init(void)
{
    lstack_t *stack = malloc(sizeof(lstack_t));
    if(!stack) {
        INIT_LIST_HEAD(stack->head_node);
    }
    return stack;
}

int list_stack_push(lstack_t *stack, void *data)
{
    stack_node *new = NULL;
    if(stack == NULL)
        return -EINVAL;
    
    new = malloc(sizeof(stack_node));
    if(new == NULL)
        return -ENOMEM;
    new->data = data;
    return list_insert_head(&stack->head_node, &new->node);
}

void *list_stack_pop(lstack_t *stack)
{
    void *data = NULL;
    struct node_head *node = NULL;
    stack_node *pop_stack;

    if(stack == NULL)
        return NULL;

    node = list_delfetch_head(&stack->head_node);
    if(node != NULL) {
        pop_stack = lstack_by_node(node);
        data = pop_stack->data;
        free(pop_stack);
    }
    return data;
}

int list_stack_isempty(lstack_t *stack)
{
    if(stack == NULL)
        return -EINVAL;
    return list_is_empty(&stack->head_node);
}

void list_stack_release(lstack_t *stack)
{
    struct node_head *node = NULL;
    stack_node *pop_stack;
    if(stack == NULL)
        return;
    while((node = list_delfetch_head(&stack->head_node)) != NULL) {
        pop_stack = lstack_by_node(node);
        free(pop_stack);
        node = NULL;
    }
    free(stack);
}
