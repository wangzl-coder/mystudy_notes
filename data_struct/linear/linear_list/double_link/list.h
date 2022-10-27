#ifndef DOUBLE_LIST_H__
#define DOUBLE_LIST_H__


#define list_for_each(head, tmp) \
        for(tmp = head->node.next; tmp != &head->node; tmp = tmp->next)

#define data_for_node(node)     (node->data)

typedef struct list_node_st{
    void *data;
    struct list_node_st *next;
    struct list_node_st *prev;
}list_node;

typedef struct list_head_st{
    int size;
    list_node node;
}list_head;

list_head *list_head_init(int size);

int list_insert_head(list_head *head, void *data);

int list_insert_tail(list_head *head, void *data);

int list_delete_head(list_head *head);

int list_delete_tail(list_head *head);

int list_update_node(list_head *head, list_node *node, void *data);

void list_release(list_head *head);


#endif
