#include "list.h"

int list_insert_head(struct node_head *head, struct node_head *new)
{
    if(head == NULL || new == NULL)
        return -EINVAL;

    new->next = head->next;
    head->next = new;
    return 0;
}

struct node_head *list_delfetch_head(struct node_head *head)
{
    struct node_head *curr;
    if(head == NULL || head->next == NULL)
        return NULL;
    
    curr = head->next;
    head->next = curr->next;
    return curr;
}

int list_is_empty(struct node_head *head)
{
    if(head == NULL)
        return -EINVAL;
    return (head->next == NULL);
}
