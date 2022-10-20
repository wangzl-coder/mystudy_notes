#include "arrlist.h"
#include <stdlib.h>

struct arrlist_st* array_list_create(int len)
{
    struct arrlist_st *list;
    if(len <= 0)
        return -NULL;

    list = malloc(sizeof(struct arrlist_st));
    if(list == NULL)
        return -NULL;

    list->data = malloc(len * sizeof(int));
    if(list->data == NULL)
        return NULL;

    list->last = 0;
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
    return 0;
}

int array_list_insert(struct arrlist_st *list, int index, int data); 

int array_list_delete(struct arrlist_st *list, int index);

int array_list_query(struct arrlist_st *list, int data);

int array_list_combine(struct arrlist_st *dest, struct arrlist_st *src);

int array_list_isempty(struct arrlist_st *list);

int array_list_set_empty(struct arrlist_st *list);

void array_list_release(struct arrlist_st *list);
