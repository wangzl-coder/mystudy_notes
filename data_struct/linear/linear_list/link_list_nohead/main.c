#include <stdio.h>
#include "stu_list.h"
#include "loop_list.h"
#include <stdlib.h>


static int list_nohead_test()
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
    return ret;
}

/* 单环链下的约瑟夫问题 */
int list_loop_jose(int total, int num, int *winner)
{
    int i, kill_num = 0, game_over = 0;
    struct node_st *fman = NULL;
    struct node_st *curr_man = NULL;
    for(i = 0; i < total; i++) {                                /* 尾插排队 */
        loop_list_insert(&fman, 1 + i, INSERT_TAIL);
    }
    i  = 1;                                                     /* 初始化报数 */
    loop_list_display(fman);
    loop_list_cycle_get(fman, curr_man) {
        if(data_of_node(curr_man) == 0) {                       /* 已经淘汰，跳过 */
            continue;
        } else {
            if(game_over) {                                     /* 已经结束，当前找到winner */
                *winner = data_of_node(curr_man);
                break;
            }
            if(i == num) {                                      /* 淘汰一名，重新报数 */
                i = 0;
                kill_num ++;
                loop_list_update_data(fman, curr_man, 0);       /* kill掉 */
                if(kill_num == (total - 1)) {                   /* 剩下一名，game over */
                    game_over = 1;
                }
            }
        }
        i++;                                                    /* 继续往后报数 */
    }

    loop_list_release(&fman);
}

int main()
{
    int ret;
    int winner;
//    ret = list_nohead_test();
    ret = list_loop_jose(6, 4, &winner);
    printf("winner is %d \r\n", winner);
    exit(ret);
}
