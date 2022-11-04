#ifndef LIST_H__
#define LIST_H__

#define INIT_LIST_HEAD(head) head.next = NULL

struct node_head{
    struct node_head *next;
};

struct node_head *list_init_head();

int list_insert_head(struct node_head *, struct node_head*);

struct node_head *list_delfetch_head(struct node_head *);

struct node_head *list_delfetch_tail(struct node_head *head);

int list_is_empty(struct node_head *);

void list_release(struct node_head *);

#endif
