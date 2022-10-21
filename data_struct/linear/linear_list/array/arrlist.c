#include <stdio.h>
#include "arrlist.h"
#include <stdlib.h>
#include <errno.h>

struct arrlist_st* array_list_create(int len)
{
    struct arrlist_st *list;
    if(len <= 0)
        return NULL;

    list = malloc(sizeof(struct arrlist_st));
    if(list == NULL)
        return NULL;

    list->data = malloc(len * sizeof(int));
    if(list->data == NULL)
        return NULL;

    list->last = 0;
    list->lenth = len;
    return list;
}

int array_list_create1(struct arrlist_st **list, int len)
{
    if(list == NULL || len <= 0)
        return -EINVAL;

    *list = malloc(sizeof(struct arrlist_st));
    if(*list == NULL)
        return -ENOMEM;

    (*list)->data = malloc(len * sizeof(int));
    if((*list)->data == NULL)
        return -ENOMEM;

    (*list)->last = 0;
    (*list)->lenth = len;
    return 0;
}

void array_list_display(struct arrlist_st *list)
{
    int i;
    if(list == NULL || list->data == NULL)
        return;

    for(i = 0; i < list->last; i++) {
        printf("%d, ", list->data[i]);
    }
}

int array_list_insert(struct arrlist_st *list, int index, int data)
{
    int i;
    if(list == NULL || list->data == NULL)
        return -EINVAL;
    
    if(index < 0 || index > list->last)
        return -EINVAL;
    
    if(list->last > list->lenth - 1)
       return -ENOSPC;

    for(i = list->last; i > index; i--) {
        list->data[i] = list->data[i - 1];
    }
    list->data[i] = data;
    list->last ++;
} 

int array_list_delete(struct arrlist_st *list, int data)
{
    int i;
    if(list == NULL || list->data == NULL)
        return -EINVAL;
    
    for(i = 0; i < list->last; i++) {
        if(list->data[i] == data)
            break;
    }
    if(i == list->last) 
        return -EINVAL;

    while(i < list->last - 1) {
        list->data[i] = list->data[i + 1];   
        i ++;
    }
    list->last --;

    return 0;
}


int array_list_query(struct arrlist_st *list, int data)
{
    int i;
    if(list == NULL || list->data == NULL)
        return -EINVAL;

    for(i = 0; i < list->last; i++) {
        if(data == list->data[i])
            break;
    }
    if(i == list->last)
        return -1;
    
    return i;
}
/*
int array_list_combine(struct arrlist_st *dest, struct arrlist_st *src);

int array_list_isempty(struct arrlist_st *list);

int array_list_set_empty(struct arrlist_st *list);
*/

void array_list_release(struct arrlist_st **list)
{
    if(list == NULL || *list == NULL)
        return ;

    if((*list)->data != NULL)
        free((*list)->data);
    (*list)->data = NULL;
    
    free(*list);
        *list = NULL;
}
