#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "loop_list.h"



int loop_list_insert(struct node_st **h_node, int data, insert_mod mode)
{
    struct node_st *new;
    if(h_node == NULL)
        return -EINVAL;
    
    if(mode != INSERT_HEAD && mode != INSERT_TAIL)
        return -EINVAL;

    new = malloc(sizeof(struct node_st));
    if(new == NULL)
        return -ENOMEM;
    
    new->data = data;
    if((*h_node) != NULL) {
        struct node_st *last = *h_node;
        while(last->next != (*h_node)) {
            last = last->next;
        }
        new->next = *h_node;
        last->next = new;
        if(mode == INSERT_HEAD)
            *h_node = new;
    } else {
        new->next = new;
        *h_node = new;
    }
}

int loop_list_update_data(struct node_st *h_node, struct node_st *up_node, int data)
{
    struct node_st *tmp = h_node;
    if(h_node == NULL || up_node == NULL)
        return -EINVAL;
    
    do{
        if(tmp == up_node) {
            tmp->data = data;
            return 0;
        }
        tmp = tmp->next;
    } while(tmp != h_node);
    
    return -EINVAL;
}


int data_of_node(struct node_st *node)
{
    if(node == NULL)
        return -1;

    return node->data;
}

void loop_list_display(struct node_st *h_node)
{
    struct node_st *tmp = h_node;
    if(tmp == NULL)
        return;

    do{
        printf("%d \n", tmp->data);
        tmp = tmp->next;
    } while(tmp != h_node);
    
    return;
}



void loop_list_release(struct node_st **h_node)
{
    struct node_st *tmp;
    if(h_node == NULL || (*h_node) == NULL)
        return;
    tmp = (*h_node)->next;
    (*h_node)->next = NULL;
    
    while(tmp != NULL) {
        struct node_st *next = tmp->next;
        free(tmp);
        tmp = next;
    }
    *h_node == NULL;
}


