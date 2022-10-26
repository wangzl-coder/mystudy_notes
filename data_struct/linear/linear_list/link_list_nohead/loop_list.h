#ifndef LOOP_LIST_H__
#define LOOP_LIST_H__



typedef enum {
    INSERT_HEAD = 0x00,
    INSERT_TAIL,
}insert_mod;



struct node_st{
    int data;
    struct node_st *next;
};


int loop_list_insert(struct node_st **, int data, insert_mod mode);


void loop_list_display(struct node_st *h_node);









#endif
