#include <stdio.h>
#include <stdlib.h>
#include "kernel_list.h"

struct student_st{
    int id;
    char name[10];
    struct list_head node;
};

int main()
{
    int i ;
    struct list_head *curr;
    struct student_st stu[6];
    struct student_st *curr_stu = NULL;
    LIST_HEAD(head);

    for(i = 0; i < sizeof(stu)/sizeof(*stu); i++)
    {
        stu[i].id = i;
        snprintf(stu[i].name, sizeof(stu[i].name),"%s-%d", "stuname",i);
        list_add_tail(&stu[i].node, &head);
    }
    printf("%-8s %-8s \r\n", "name", "id");
    list_for_each(curr, &head) {
        curr_stu = list_entry(curr, struct student_st, node);
        printf("%-8s %-8d \r\n", curr_stu->name, curr_stu->id);
    }

    exit(0);
}
