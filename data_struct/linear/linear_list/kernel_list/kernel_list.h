#ifndef KERNEL_LIST_H__
#define KERNEL_LIST_H__

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define offsetof(type, member) ((size_t) &((type*) 0)->member)

#define container_of(ptr, type, member) ({      \
        const typeof( ((type *)0)->member)  *__mptr = (ptr); \
        (type *)( (char*)__mptr - offsetof(type, member) ); })

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

struct list_head{
    struct list_head *next;
    struct list_head *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}


static inline void __list_add(struct list_head *new, 
                        struct list_head *prev, struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void __list_del_head(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}


#endif
