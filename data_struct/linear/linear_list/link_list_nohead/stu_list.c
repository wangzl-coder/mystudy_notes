#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "stu_list.h"


int student_insert_head(struct stu_node **node, struct student_st *pst_info)
{
    struct stu_node *new_stu;
    if(node == NULL || pst_info == NULL)
       return -EINVAL;

    new_stu = malloc(sizeof(struct stu_node));
    if(new_stu == NULL)
        return -ENOMEM;
    
    new_stu->next = *node;
    new_stu->stu_info = *pst_info;
    *node = new_stu;

    return 0;
}

int student_insert_tail(struct stu_node **node, struct student_st *pst_info)
{
    struct stu_node *new_stu, *tmp;
    if(node == NULL || pst_info == NULL)
       return -EINVAL;
    
    tmp = *node;
    new_stu = malloc(sizeof(struct stu_node));
    if(new_stu == NULL)
        return -ENOMEM;
    
    new_stu->stu_info = *pst_info;
    new_stu->next = NULL;
    if(*node == NULL) {
        *node = new_stu;
        return 0;
    }
    while(tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_stu;
}

int student_delete_head(struct stu_node **node)
{
    struct stu_node *tmp;
    if(node == NULL || *node == NULL)
        return -EINVAL;
    
    tmp = *node;
    *node = (*node)->next;
    tmp->next = NULL;
    free(tmp);
    return 0;
}

int student_delete_tail(struct stu_node **node)
{
    struct stu_node *tmp, *pre = NULL;
    if(node == NULL || *node == NULL)
        return -EINVAL;
    
    tmp = *node;
    while(tmp->next != NULL) {
        pre = tmp;
        tmp = tmp->next;
    }
    if(pre == NULL)
        *node = pre;
    else
        pre->next = NULL;
    free(tmp);
    return 0;
}

int student_query_byid(struct stu_node *node, struct student_st *pst_info)
{
    


}

void student_info_display(struct stu_node *node)
{
    struct student_st stu;
    printf("%-8s %-13s %-8s %-8s %-8s\n\n", "id", "name", "gender", "age", "class");
    while(node != NULL) {
        stu = node->stu_info;
        printf("%-8d %-13s %-8s %-8d %-8d\n", stu.id, stu.name, stu.gender, stu.age, stu.class);
        node = node->next;
    }
}
//int student_update_info(struct stu_node *node, struct student_st);

void students_release(struct stu_node **node)
{
    struct stu_node *tmp;
    if(node == NULL || *node == NULL)
        return;
    
    while(*node != NULL) {
        tmp = (*node)->next;
        (*node)->next = NULL;
        free(*node);
        *node = tmp;
    }
}
