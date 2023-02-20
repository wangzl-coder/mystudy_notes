#if 0
    链表：
        
        1 单向链表和双向链表
            struct list_element {
                void *data;
                struct list_element *next;
            }
            
            struct list_element {
                void *data;
                struct list_element *next;
                struct list_element *prev;
            }
        
        2 环形链表:

        3 沿链表移动：
            头指针：

        4 Linux内核中的实现：不将数据结构塞入链表，二十九按链表结构塞入数据结构
            （1）链表数据结构：
                struct list_head {
                    struct list_head *next;
                    struct list_head *head;
                }
            container_of():从链表指针找到父结构体中包含的任何变量
            container_of(ptr, type, member) ({                                      \
                const typeof(((type*)0)->member) *__mptr = (ptr);                   \
                (type*)( (char *)__mptr - offsetof(type, member) );})               \

#define list_entry(ptr, type, member)           \
                container_of(ptr, type, member);

            （2）定义一个链表
            struct fox {
                unsigned long tail_length;
                unsigned long weight;
                bool is_fantastic;
                struct list_head list;
            }
            struct fox *red_fox;
            red_fox = kmalloc(sizeof(*red_fox), GFP_KERNEL);
            red_fox->tail_length = 40;
            red_fox->weight = 6;
            red_fox->is_fantastic = false;
            INIT_LIST_HEAD(&red_fox->list);

            struct fox red_fox = {
                .tail_length = 40;
                .weight = 6;
                .list = LIST_HEAD_INIT(red_fox.list);
            };

            （3）链表头：使用一个标准的索引指针指向整个链表，即链表的头指针
                static LIST_HEAD(fox_list);
        

        5 操作链表：所有操作的时间复杂度O(1)
            
            （1）向链表增加一个节点：
                list_add(struct list_head *new, struct list_head *head)
                头插，向head节点后添加new节点

                list_add_tail(struct list_head *new, struct list_head *head)
                尾插，向head节点前添加new节点

            （2）从链表删除一个节点：
            list_del(struct list_head *entry);
            
            static inline void __list_del(struct list_head *prev, struct list_head *next)
            {
                prev->next = next;
                next->prev = prev;
            }
    
            static inline void list_del(struct list_head *entry)
            {
                __list_del(entry->prev, entry->next);
            }
            
            从链表中删除一个节点并对其重新初始化
            list_del_init();
            list_del_init(struct list_head *entry);

            （3）移动和合并链表节点
            把节点从一个链表移到另一个链表：
            list_move(struct list_head *list, struct list_head *head)
            从一个链表中移除list项，并将其加入到另一链表的head节点后面。

            把节点从一个链表移到另一个链表的末尾：
            list_move_tail(struct list_head *list, struct list_head *head)
            从一个链表中移除list项，并将其加入到另一链表head节点前面

            检查链表是否为空：
            list_empty(struct list_head *head);

            把两个链表合并在一起：
            list_splice(struct list_head *list, struct list_head *head)
            合并两个链表，将list指向的链表插入到指定链表的head元素后面

            把两个未连接的链表合并在一起，并重新初始化原来的链表
            list_splice_init(struct list_head *list, struct list_head *head)
            将list指向的链表插入到指定链表head元素后面，并重新初始化list指向的链表



            6 遍历链表：

#endif
