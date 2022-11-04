#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "stack.h"


//#define CALCULATOR_DEBUG
#ifdef CALCULATOR_DEBUG
#define calcula_debug(format, arg...) printf("[cal][%s:%d]:"format, __FUNCTION__, __LINE__, ##arg);
#else
#define calcula_debug(format, arg...)
#endif


#define ptr_is_symbol(ptr) (*(ptr) == '+' || \
                            *(ptr) == '-' || \
                            *(ptr) == '*' || \
                            *(ptr) == '/' )

#define ptr_is_num(ptr) (*(ptr) >= '0' && *(ptr) <= '9')

#define SYMBOL_PRIOTY_0 (0)
#define SYMBOL_PRIOTY_1 (1)


struct calparam_st{
    int is_symbol;
    int param;
};

static char *calcu_getparam(char *buff, struct calparam_st *cal_param)
{
    char *ptr = buff;
    int is_num = 0;

    if(buff == NULL || cal_param == NULL)
        return NULL;
    
    if(*buff == '\0')
        return NULL;
    
    if(ptr_is_symbol(ptr)) {    //symbol. one byte
        cal_param->is_symbol = 1;
        cal_param->param = *ptr++;
    } else {
        cal_param->is_symbol = 0;
        cal_param->param = 0;
        while(*ptr != '\0') {
            if(ptr_is_num(ptr)) {
                   cal_param->param = cal_param->param*10 + (*ptr++) - '0';
            } else 
                break;
        }
    
    }
    return ptr;
}

static inline int calculator_num(int leftnum, int rightnum, int symbol)
{
    int retval;
    switch(symbol) {
    
        case '+':
            retval = leftnum + rightnum;
            break;
        case '-':
            retval = leftnum - rightnum;
            break;
        case '*':
            retval = leftnum * rightnum;
            break;
        case '/':
            retval = leftnum / rightnum;
            break;
        default:
            retval = 0;
    }
    return retval;
}

static int cal_stack_push(lstack_t *stack, int val)
{
    int *p_num = malloc(sizeof(int));
    if(p_num == NULL)
        return -EINVAL;

    *p_num = val;
    return list_stack_push(stack, p_num);
}

static int cal_stack_pop(lstack_t *stack, int *val)
{
    int *p_num = list_stack_pop(stack);
    
    if(p_num == NULL) {
        return -EINVAL;
    }
    *val = *p_num;
    free(p_num);
}

static int get_priority_for_symbol(int symbol)
{
    int retval;
    switch(symbol) {
        case '+':
        case '-':
            retval = SYMBOL_PRIOTY_0;
            break;
        case '*':
        case '/':
            retval = SYMBOL_PRIOTY_1;
            break;
        default:
            retval = -1;
            break;
    }
    return retval;
}

static int compare_symbol_priority(int symbol1, int symbol2)
{
    int p1, p2;
    p1 = get_priority_for_symbol(symbol1);
    p2 = get_priority_for_symbol(symbol2);
   return get_priority_for_symbol(symbol1) - get_priority_for_symbol(symbol2);
}

int calculatotr_by_str(char *str, int *val)
{
    char *ptr;
    int retval = 0;
    struct calparam_st calparam;
    lstack_t *num_stack;
    lstack_t *symbol_stack;
    
    if(str == NULL || val == NULL)
        return -EINVAL;


    num_stack = list_stack_init();
    if(num_stack == NULL) {
        return -ENOMEM;
    }
    symbol_stack = list_stack_init();
    if(symbol_stack == NULL) {
        retval = -ENOMEM;
        goto nstack_release;
    }

    for(ptr = str; ptr != NULL;) {
        int top_num;
        int top_symbol;
        ptr = calcu_getparam(ptr, &calparam);
        if(!calparam.is_symbol) {                       /* get num */
            if(*ptr == '\0') {                          /* for tail,pop stack and calculate */
                calcula_debug("*******\r\n");
                *val = calparam.param;
                do {
                    cal_stack_pop(num_stack, &top_num);
                    cal_stack_pop(symbol_stack, &top_symbol);
                    *val = calculator_num(top_num, *val, top_symbol);
                }while(!(list_stack_isempty(num_stack) || list_stack_isempty(symbol_stack)));
                calcula_debug("final result : %d \r\n", retval);
                break;
            } else 
            {                                    /* push num */
                cal_stack_push(num_stack, calparam.param);
                calcula_debug("push num %d \r\n", calparam.param);
            }
        } else {    /* get symbol */
            int leftnum, rightnum;
            if(list_stack_isempty(symbol_stack)) {      /* symbol empty,just push */
                cal_stack_push(symbol_stack, calparam.param);
                calcula_debug("push symbol  %c \r\n", calparam.param);
                continue;
            }
            /* have symbol stack,pop and compare priority */
            cal_stack_pop(symbol_stack, &top_symbol);
            if(compare_symbol_priority(top_symbol, calparam.param) >= 0) {  /* stack has high(equ) priority,pop and calculate */
                int result;
                cal_stack_pop(num_stack, &rightnum);
                cal_stack_pop(num_stack, &leftnum);
                result = calculator_num(leftnum, rightnum, top_symbol);
                calcula_debug("pop calcula %d%c%d->result=%d \r\n", leftnum,top_symbol,rightnum, result);
                calcula_debug("push %d \r\n", result);
                cal_stack_push(num_stack, result);
                calcula_debug("push symbol %c \r\n", calparam.param);
                cal_stack_push(symbol_stack, calparam.param);
            } else {                                                        /* curr symbol has higher priority,push */
                cal_stack_push(symbol_stack, top_symbol);                                                           
                calcula_debug("push symbol %c \r\n", calparam.param);
                cal_stack_push(symbol_stack, calparam.param);                                                           
            }
        }
    }

    list_stack_release(symbol_stack);
nstack_release:
    list_stack_release(num_stack);
    return retval;
}


