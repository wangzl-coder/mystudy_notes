#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"



static int list_stack_simple_demo()
{
    int i, ret = 0;
    int a[] = {1,2,3,4,5,6,7,8,9};
    lstack_t *stack = list_stack_init();
    if(stack == NULL) {
        fprintf(stderr, "stack init faied \r\n");
        return -1;
    }
    for(i = 0; i < sizeof(a)/sizeof(*a); i++) {
        ret = list_stack_push(stack, &a[i]);
        if(ret < 0) {
            fprintf(stderr, "list stack push failed. ret=%d \r\n",ret);
            break;
        }
    }
    for(i = 0; i < sizeof(a)/sizeof(*a); i++) {
        int *a = NULL;
        a = list_stack_pop(stack);
        if(a != NULL) {
            printf("pop : %d \r\n", *a);
        }
    }
    list_stack_release(stack);
    return ret;
}

static int stack_calculator_demo(char *buff)
{
    int val;
    calculatotr_by_str(buff, &val);
    return val;
}

int main(int argc, char *argv[])
{
    char buff[50]; 
    while(1) {
        scanf("%s",buff);
        if(strcmp(buff, "quit") == 0){ 
            break;
        }
        printf("%s = %d\r\n",buff, stack_calculator_demo(buff));
    }
    exit(0);
}
