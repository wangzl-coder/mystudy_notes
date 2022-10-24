#include <stdlib.h>
#include "list.h"


node_head *list_init_head(int size)
{
    node_head *head = NULL;
    if(size <= 0)
        return -EINVAL;

    head = malloc(sizeof(node_head));
    if(head == NULL)
        return -ENOMEM;

    head->size = size;
    head->head_node.data = NULL;
    head->head_node.next = NULL;
    return head;
}

int list_add_head(node_head *head, void *data)
{
    node *new_node = NULL;
    if(head == NULL || data == NULL)
        return -EINVAL;

    new_node = malloc(sizeof(node));
    if(new_node == NULL)
        return -ENOMEM;

    new_node->data = malloc(head->size);
    if(new_node->data == NULL) {
        free(new_node);
        return -ENOMEM;
    }
    memcpy(new_node->data, data, head->size);
    new_node->next = head->head_node.next;
    head->head_node.next = new_node;
    return 0;
}

int list_add_tail(node_head *head, void *data)
{
    node new_node, *last = NULL;
    if(head == NULL || data == NULL)
       return -EINVAL;

    new_node = malloc(sizeof(node));
    if(new_node == NULL)
        return -ENOMEM;
    new_node->data = malloc(head->size);
    if(new_node->data == NULL) {
        free(new_node);
        return -ENOMEM;
    }
    last = head->head_node.next;
    new_node->next = NULL;
    memcpy(new_node->data, data, head->size);
    if(last != NULL) {
        while(last->next != NULL) {
            last = last->next;
        }
        last->next = new_node;
    } else {
        head->head_node.next = new_node;
    }
    return 0;
}

int list_delete_head(node_head *head)
{
    node *tmp;
    if(head == NULL)
        return -EINVAL;
    
    tmp = head->head_node.next;
    if(tmp != NULL) {
        head->head_node.next = tmp->next;
        free(tmp->data);
        free(tmp);
    }
    return 0;
}

int list_delete_tail(node_head *head)
{
    node *last, pre = NULL;

    last = head->head_node.next;
    if(last != NULL) {
        while(last->next != NULL) {
            pre = last;
            last = last->next;
        }
        if(pre != NULL) {
            pre->next = NULL;
        } else {
            head->head_node.next = NULL;
        }
        free(last->data);
        free(last);
    }
    return 0;
}

void list_release(node_head **head)
{
    node *curr, *next = NULL;
    if(head == NULL)
        return -EINVAL;
    
    for(curr = (*head)->head_node.next; curr != NULL; curr = next) {
        next = curr->next;
        free(curr->data);
        free(curr);
    }
    free(*head);
    *head = NULL;
}
