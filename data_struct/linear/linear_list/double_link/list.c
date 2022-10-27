#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "list.h"


list_head *list_head_init(int size)
{
    list_head *head = malloc(sizeof(list_head));
    
    if(head != NULL) {
        head->size = size;
        head->node.next = &head->node;
        head->node.prev = &head->node;
    }
    return head;
}


static int list_insert(list_node *p, list_node *new, list_node *q)
{
    p->next = new;
    new->prev = p;
    new->next = q;
    q->prev = new;

    return 0;
}

int list_insert_head(list_head *head, void *data)
{
    list_node *new;
    if(head == NULL || data == NULL)
        return -EINVAL;
    
    new = malloc(sizeof(list_node));
    if(new == NULL)
        return -ENOMEM;

    new->data = malloc(head->size);
    if(new->data == NULL) {
        free(new);
        return -ENOMEM;
    }
    memcpy(new->data, data, head->size);
    /*
    head->node.next->prev = new;
    new->next = head->node.next;
    head->node.next = new;
    new->prev = &head->node;
    */
    list_insert(&head->node, new, head->node.next);
}

int list_insert_tail(list_head *head, void *data)
{
    list_node *new;
    if(head == NULL || data == NULL)
        return -EINVAL;

    new = malloc(sizeof(list_node));
    if(new == NULL)
        return -EINVAL;
    new->data = malloc(head->size);
    if(new->data == NULL) {
        free(new);
        return -EINVAL;
    }
    memcpy(new->data, data, head->size);
    /*
    head->node.prev->next = new;
    new->prev = head->node.prev;
    new->next = &head->node;
    head->node.prev = new;
    */
    list_insert(head->node.prev, new, &head->node);
}

static int list_delete(list_node *delete)
{
    list_node *next_node = delete->next;
    delete->next->prev = delete->prev;
    delete->prev->next = delete->next;
    free(delete->data);
    delete->data = NULL;
    free(delete);
    delete = NULL;
}

int list_delete_head(list_head *head)
{
    list_node *delete_node;
    if(head == NULL)
        return -EINVAL;
    if(head->node.next != &head->node) {
        delete_node = head->node.next;
        /*
        list_node *next_node = delete_node->next;
        head->node.next = next_node;
        next_node->prev = &head->node;
        free(delete_node->data);
        delete_node->data = NULL;
        free(delete_node);
        delete_node = NULL;
        */
        list_delete(delete_node);
    }
    return 0;
}

int list_delete_tail(list_head *head)
{
    list_node *delete_node;
    if(head == NULL)
        return -EINVAL;

    if(head->node.next != &head->node) {
        delete_node = head->node.prev;
        /*
        list_node *prev_node  = delete_node->prev;
        prev_node->next = &head->node;
        head->node.prev = prev_node;
        free(delete_node->data);
        delete_node->data = NULL;
        free(delete_node);
        delete_node = NULL;
        */
        list_delete(delete_node);
    }
    return 0;
}

int list_update_node(list_head *head, list_node *node, void *data)
{
    list_node *curr;
    if(head == NULL || node == NULL || data == NULL)
        return -EINVAL;
    
    curr = &head->node;
    do{
        curr = curr->next;
        if(curr == node){
            memcpy(curr->data, data, head->size);
            break;
        }

    } while(curr != &head->node);
    
    return (curr == &head->node);
}

void list_release(list_head *head)
{
    list_node *curr, *next = NULL;
    if(head == NULL)
        return ;
    
    for(curr = head->node.next; curr != &head->node; curr = next) {
        next = curr->next;
        free(curr->data);
        free(curr);
    }
    free(head);
}
