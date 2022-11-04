#ifndef SQ_STACK_H__
#define SQ_STACK_H__


#define MAXSIZE (20)

typedef struct charact_st{
    char *name;
    int id;
}datatype;

typedef struct stack_st{
    datatype data[MAXSIZE];
    int top;
}sq_stack;


sq_stack *stack_init(void);

int stack_is_empty(sq_stack *);

int stack_push(sq_stack *stack, datatype *data);

datatype *stack_pop(sq_stack *);

void stack_release(sq_stack *);


#endif
