#include <stdio.h>
#include <stdlib.h>
#include "sqstack.h"


static int sqstack_demo_test(void)
{
    int i, len;
    sq_stack *stack = NULL;
    int ret = 0;
    struct charact_st stu[] = {
        {.name = "wangzl", .id = 888},
        {.name = "zhangsan", .id = 6},
        {.name = "lisi", .id = 33},
        {.name = "xiaoming", .id = 298},
        {.name = "xiaohong", .id = 900},
        {.name = "john", .id = 823},
    };
    len = sizeof(stu)/sizeof(struct charact_st);
    stack = stack_init();
    if(stack == NULL) {
        puts("stack init failed");
        return -1;
    }
    for(i = 0; i < len; i++) {
        ret = stack_push(stack, &stu[i]);
        if(ret < 0) {
            fprintf(stderr, "stack push failed i = %d,ret = %d\r\n", i, ret);
            goto stack_release;
        }
    }
    printf("%-8s %-8s \r\n", "name", "id");
    for(i = 0; i < len; i++) {
        struct charact_st *tmp = stack_pop(stack);
        if (tmp == NULL) {
            ret = -1;
            fprintf(stderr, "stack pop failed, i = %d \r\n", i);
            break;
        }
        printf("%-8s %-8d \r\n", tmp->name, tmp->id);
    }
    
stack_release:
    stack_release(stack);
    return ret;
}

int main()
{
    int ret;
    
    ret = sqstack_demo_test();
    if(ret < 0)
        fprintf(stderr, "sqstack demo test exceprion occurred \r\n");
    exit(ret);
}
