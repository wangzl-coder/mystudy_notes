#if 0
    tasklet: 和软中断本质上很相似，但其接口更简单，锁保护要求较低，通常应该使用tasklet。


        1 tasklet的实现：
            tasklet通过软中断实现，本身也是软中断。由HI_SOFTIRQ和TASKLET_SOFTIRQ表示。

            （1）tasklet结构体：
                struct tasklet_struct {
                    struct tasklet_struct *next;    /* 链表中的下一个tasklet */
                    unsigned long state;            /* tasklet的状态 */
                    atomic_t count;                 /* 引用计数器 */
                    void (*func)(unsigned long );   /* tasklet处理函数 */
                    unsigned long data;             /* 给tasklet处理函数的参数 */
                }
#endif
