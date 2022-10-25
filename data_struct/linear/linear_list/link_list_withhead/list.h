#ifndef LINK_LIST_H__
#define LINK_LIST_H__

/* 将数据域抽象出来 */

typedef struct node_st{
    void *data;
    struct node_st *next;
}node;

typedef struct{
    int size;
    node head_node;
}node_head;

#define list_for_each_node(head, curr) \
        for(curr = head->head_node.next; curr != NULL; curr = curr->next)

#define data_by_node(node, type)  ((type*) node->data)

node_head *list_init_head(int size);

int list_add_head(node_head *head, void *data);

int list_add_tail(node_head *head, void *data);

int list_update_node(node_head *head, node *node, void *data);

int list_delete_head(node_head *head);

int list_delete_tail(node_head *head);

void list_release(node_head **head);

#endif
