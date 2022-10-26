#ifndef LOOP_LIST_H__
#define LOOP_LIST_H__


#define loop_list_cycle_get(h_node, curr_node)  \
            for(curr_node = h_node; ; curr_node = curr_node->next)

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


int loop_list_update_data(struct node_st *h_node, struct node_st *up_node, int data);

int data_of_node(struct node_st *node);


void loop_list_release(struct node_st **h_node);





#endif
