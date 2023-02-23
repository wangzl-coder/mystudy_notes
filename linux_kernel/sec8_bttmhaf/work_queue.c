#if 0
    工作队列：
        工作队列（work queue） 是另一种将工作推后执行的形式，交由一个内核线程去执行，这个下半部总
            是会在进程上下文执行，所以，工作队列允许重新调度甚至睡眠。

        工作队列是唯一能在进程上下文中运行的下半部实现机制

        工作者线程：驱动程序创建专门的工作者线程处理下半部工作

        缺省的工作者线程events/n：工作队列子系统提供通用的工作者线程处理，把需要推后执行的任务交给特定的通用线程


        1 表示线程的数据结构：
            工作者线程用workqueue_struct表示：
            /* 外部可见的工作队列是由每个CPU的工作队列组成的数组 */
                struct workqueue_struct {
                    sturct cpu_workqueue_struct cpu_wq[NR_CPUS];
                    struct list_head list;
                    const char *name;
                    int sinqlethread;
                    int freezeable;
                    int rt;
                };
            每个处理器，每个工作者线程对应一个cpu_workqueue_struct 结构体：
                struct cpu_workqueue_struct {
                spin_lock lock; /* 锁保护 */
                struct list_head worktlist; /* 工作列表 */
                wait_queue_head_t more_work;
                struct work_struct *current_struct;
                struct workqueue_struct *wq; /* 关联工作队列结构 */
                task_t thread;  /* 关联线程 */
                };
            每个工作者线程类型关联一个workqueue_struct .每个处理器都有一个该类型的工作者线程
        
        2 表示工作的数据结构
            所有的工作者线程都是用普通的内核线程实现的，无工作时，睡眠，有工作时线程被唤醒，以便执行这些操作
            工作表示结构：
            struct work_struct {
                atomic_long_t data;
                struct list_head entry;
                work_func_t func;
            };
            结构被连接成链表，每个处理器的每种类型的队列都对应这样一个链表，工作者线程会执行其链表上的所有工作：
            for(; ;) {
                prepare_to_wait(&cwq->more_work, &wait, TASK_INTERRUPTIBLE);
                if(list_empty(&cwq->work_list))
                    schedule();
                finish_wait(&cwq->more_work, &wait);
                run_workqueue(cwq);
            }
            (1) 将自己设置为休眠状态，并加入到等待队列
            (2) 入股工作链表是空的，调用schedule()函数进入睡眠状态
            (3) 如果链表中有对象，线程不会睡眠，将设置成TASK_RUNNING，脱离等待队列
            (4) 如果链表非空，调用run_workqueue()函数执行被推后的工作：
            while(!list_empty(&cwq->worklist)) {
                struct work_struct *work;
                work_func_t f;
                void *data;

                work = list_entry(cwq->worklist.next, struct work_struct, entry);
                f = work->func;
                list_del_init(cwq->worklist.next);
                work_clear_pending(work);
                f(work);
            }
            (1) 当链表不为空时，获取下一个节点对象
            (2) 获取我们希望执行的func以及参数data；
            (3) 把该节点从链表上解下来，将待处理标志位pending清零
            (4) 调用函数
            (5) 重复执行

        3 工作队列实现机制的总结:
            系统允许有多种类型的工作者线程存在，对于指定的类型，系统每个CPU上都有一个该类的工作者线程workqueue_struct，
            工作处于最底层，驱动程序创建这些需要推迟执行的工作work_struct,工作会被提交给某个具体的工作者线程，这个工作
            者线程会被唤醒并执行这些安排好的工作。


        4 使用工作队列：
            （1）创建推后的工作：编译时静态创建：
                DECLARE_WORK(name, void (*func)(void *), void *data);
                
                运行时指针创建：
                INIT_WORK(struct work_struct *work, void (*func)(void *), void *data);

            （2）工作队列处理函数：
                void work_handler(void *data);
                函数由工作者线程执行，因此，函数运行在进程上下文。默认情况下，允许响应中断，并且不会持有任何锁，如果需要，
                函数可以睡眠（但不能访问用户空间，内核线程没有用户空间的内存映射）。
            
            （3）对工作进行调度（提交给缺省的events工作线程）：
                shedule_work(&work);
                希望延迟一段时间后执行：
                schedule_dealyed_work(&work, delay);

            (4) 刷新操作：有时需要保证操作已经执行完毕：
                void flush_scheduled_work(void);
                函数会等待直到队列种所有对象被执行后返回，该函数等待时进入休眠状态，因此，只能在进程上下文调用，
                函数不会取消任何延迟执行的工作

                int cancel_delayed_work(struct work_struct *work);
                可以取消延迟执行的工作。

        5 创建新的工作队列（需要明确必须靠自己的线程来提高性能）：
            创建一个新的任务队列和与之相关的工作者线程：
            struct workqueue_struct *create_workqueue(const char *name);
            @name: 该内核线程的命名
            会创建所有的工作者线程（系统中每个处理器都有一个），创建工作之后，调用：
            int queue_work(srtuct workqueue_struct *wq, struct work_struct *work);
            或者
            int queue_delayed_work(struct workqueue_struct *wq, struct work_struct *work, unsigned long delay);

            刷新指定的工作队列：
            flush_workqueue(struct workqueue_struct *wq);

#endif
