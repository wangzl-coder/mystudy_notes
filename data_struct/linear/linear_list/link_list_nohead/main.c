#include <stdio.h>
#include "stu_list.h"


int main()
{
    int i, ret;
    struct stu_node *pstu_node = NULL;
    struct student_st stu[] = {
        {.id = 1024,    .name = "wangzl",       .age = 22, .gender = "male",    .class = 12},
        {.id = 1023,    .name = "wanghx",       .age = 21, .gender = "female",  .class = 11},
        {.id = 289,     .name = "mayiming",     .age = 22, .gender = "male",    .class = 12},
        {.id = 1000,    .name = "gaojianhua",   .age = 23, .gender = "male",    .class = 13},
        {.id = 999,     .name = "zhangling",    .age = 20, .gender = "female",  .class = 11},
        {.id = 888,     .name = "lizi",         .age = 25, .gender = "female",  .class = 14},
    };
    for(i = 0; i < sizeof(stu)/sizeof(*stu); i++) {
//        ret = student_insert_head(&pstu_node, &stu[i]);
        ret = student_insert_tail(&pstu_node, &stu[i]);        
        if(ret < 0) {
            puts("inster student failed");
            break;
        }
    }
    student_info_display(pstu_node);
    ret = student_delete_head(&pstu_node);
    ret = student_delete_tail(&pstu_node);
    puts("-----------------------------------");
    student_info_display(pstu_node);
    students_release(&pstu_node);
}
