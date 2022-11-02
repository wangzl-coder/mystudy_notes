#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "sqstack.h"



sq_stack *stack_init(void)
{
    sq_stack *stack = malloc(sizeof(sq_stack));
    if(stack)
        stack->top = MAXSIZE;
    
    return stack;
}

int stack_is_empty(sq_stack *stack)
{
    return (stack->top == MAXSIZE);
}

int stack_push(sq_stack *stack, datatype *data)
{
    if(stack == NULL || data == NULL)
        return -EINVAL;
    
    if((--stack->top) < 0)
        return -ENOSPC;

    memcpy(&stack->data[stack->top], data, sizeof(datatype));
    return 0;
}

datatype *stack_pop(sq_stack *stack)
{
    if(stack == NULL)
        return NULL;

    if(stack->top > MAXSIZE - 1)
        return NULL;

    return &(stack->data[stack->top ++]);
}

void stack_release(sq_stack *stack)
{
    free(stack);
}
