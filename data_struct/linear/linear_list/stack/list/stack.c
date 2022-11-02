#include "stack.h"

lstack_t *list_stack_init(void)
{
    lstack_t *stack = NULL;
    if(size <= 0)
        return NULL;

    stack = malloc(sizeof(lstack_t));
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
    
    if(stack == NULL)
        return NULL;

    

}

int list_stack_isempty(lstack_t *);

int list_stack_release(lstack_t *);
