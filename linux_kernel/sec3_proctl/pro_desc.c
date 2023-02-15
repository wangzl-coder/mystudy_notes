#if 0
    1 进程描述符及任务结构：
        任务队列和进程描述符：双向循环链表，每一项为task_struct,称为进程描述符，包含一个具体进程的所有信息：打开的文件，
            进程的地址空间，挂起的信号，进程的状态等。
            struct task_struct {
                unsigned long state;
                int prio;
                unsigned long policy;
                struct task_struct *parent;
                struct list_head *tasks;
                pid_t pid;
                ...
            };
    
    2 分配进程描述符
        （1）通过slab分配器分配task_struct 结构，在栈底（向下增长的栈）或栈顶（向上增长的栈）创建新的结构struct thread_info
            struct thread_info {
                struct task_struct *task;
                struct exec_domain *exec_domain;
                __u32 flags;
                __u32 status;
                __u32 cpu;
                int preempt_count;
                mm_segment_t addr_limit;
                struct restart_block restart_block;
                void *sysenter_return;
                int uaccess_err;
            }

    3 进程描述符的存放
            进程标识值：pid_t
            进程描述符指针的存放：current宏来设定获取当前正在执行的进程，可通过专门的寄存器或者栈指针偏移计算（根据具体体系架构）

    4 进程状态：
        TASK_RUNNING(运行)：正在执行或在运行队列中等待执行，进程在用户空间执行的唯一可能状态
        TASK_INTERRUPTIBLE（可中断睡眠）：等待条件达成，会被信号唤醒提前投入运行
        TASK_UNINTERRUPTIBLE(不可中断睡眠)：等待条件达成，不会被信号打断唤醒
        __TASK_TRACES: 被其他进程跟踪的进程，如通过ptrace对调试程序进行跟踪
        __TASK_STOP: 停止状态，没有投入运行也不能投入运行，通常在接收到SIGSTOP, SIGTSTP, SIGTTIN, SIGTTOU等信号的时候
    
    5 设置当前进程状态：
        set_task_state(task, state);

    6 进程上下文：
        程序执行系统调用或者触发某个异常，将陷入内核空间，此时内核“代表进程执行”并处于进程上下文中，在此上下问current宏有效，除非重新调度，
        否则在内核退出时，程序恢复在用户空间的执行。

    7 进程家族树
        (1) init进程：所有的进程都是pid为1的init进程的后代，内核在系统启动的最后阶段启动init进程，改进程读取初始化脚本并执行其他的相关程序。
        (2) 兄弟进程和父子进程：
                struct task_struct *my_parent = current->parent;

            访问子进程：
                struct task_struct *task;
                struct list_head *list;
                list_for_each(list, &current->childrec) {
                    task = list_entry(list, struct task_struct, sibling);
                }

        (3) init进程的进程描述符作为init_task静态分配：
            truct task_struct *task;
            fot(task = current; task != &init_task; task = task->parent)

        (4) 可以从继承体系从系统的任何一个进程出发查找到任意指定的其他进程，获取链表的下一个进程
            list_entry(task->task.next, struct task_struct, tasks);

            获取前一个进程：
            list_entry(task->task.prev, struct task_struct, tasks);

#endif
