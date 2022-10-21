#ifndef ARRAY_LIST_H__
#define ARRAY_LIST_H__


struct arrlist_st{
    int *data;   
    int lenth;
    int last;
};

struct arrlist_st* array_list_create(int len);

int array_list_create1(struct arrlist_st **list, int len);

int array_list_insert(struct arrlist_st *list, int index, int data); 

int array_list_delete(struct arrlist_st *list, int index);

int array_list_query(struct arrlist_st *list, int data);

int array_list_combine(struct arrlist_st *dest, struct arrlist_st *src);

int array_list_isempty(struct arrlist_st *list);

int array_list_set_empty(struct arrlist_st *list);

void array_list_display(struct arrlist_st *list);

void array_list_release(struct arrlist_st **list);


#endif
