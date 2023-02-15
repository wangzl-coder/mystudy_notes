#if 0
    自身终结（exit()）和被动终结（信号和异常），终结任务考do_exit()进行：
        (1) 将task_struct 中的 flags 设置为 PF_EXITING,
        (2) 调用del_timer_sync()删除任一内核定时器，根据返回结果确保没有定时器排队也没有定时器处理程序在运行
        (3) 如果BSD进程记账功能开启，调用acct_update_integrals()来输出记账信息
        (4) 调用exit_mm()函数释放进程占用的mm_struct,如果没有别的进程使用，就彻底释放
        (5) 调用sem_exit()函数，如果进程排队等候IPC信号，则离开队列。
        (6) 调用exit_files()和exit_fs()，递减文件描述符，文件系统数据引用，如果引用值降为0，则可以释放
        (7) 将task_struct 的exit_code成员置为由exit()提供的退出代码
        (8) 调用exit_notify()向父进程发送信号，给子进程重新找养父，线程组其他线程或init进程，状态设置为EXIT_ZOMBIE
        (9) 调用schedule()切换到新的进程

    删除进程描述符：尽管线程僵死不在运行，但系统保留其进程描述符。在父进程获得已终结的子进程信息后，子进程的task_struct被释放
        wait()函数族通过唯一的系统调用wait4()实现，标准动作：挂起调用进程，直到其中一个子进程退出，返回该子进程的pid，调用该函数
        时提供的指针会包含子函数退出时的退出代码
        释放文件描述符由release_task（）实现：
            (1) 调用__exit_signal(),该函数调用_unhash_process(),又调用detach_pid()从pidhash上删除该进程，同时从任务列表中删除该进程
            (2) _exit_signal()释放目前僵死进程所使用的所有剩余资源，并进行最终统计和记录
            (3) 如果这个进程时线程组的最后一个进程，并且领头进程已经死掉，那么release_task()就要通知僵死的领头进程的父进程
            (4) release_task()调用put_task_struct()释放进程内核栈和thread_info结构所占领的页，并释放task_struct 所占的slab高速缓存
        
    孤儿进程造成的进退维谷：
        （1）父进程在子进程前退出，将给子进程在当前线程组内找一个线程作为父亲，如果不行，就让init做它们的父进程，在do_exit()中会调用
            exit_notify(),该函数会调用forget_original_parent(),而后者会调用find_new_reaper()来执行寻父过程：
#endif
/*
 * 试图找到进程所在的线程组内的其他进程，如果线程组内没有其他进程，他就找到并返回的是init进程
 *
 * */
static struct task_struct *find_new_reaper(struct task_struct *father)
{
    struct pid_namespace *pid_ns = task_active_pid_ns(father);
    struct task_struct *thread;
    thread = father;
    while_each_thread(father, thread) {
        if(thread->flags & PF_EXITING)
            continue;
        if(unlikely(pid_ns->child_reaper == father))
            pid_ns->child_reaper = thread;
        return thread;
    }
    
    if(unlikely(pid_ns->child_reaper) == father) {
        write_unlock_irq(&tasklist_lock);
        if(unlikely(pid_ns == &init_pid_ns))
            panic("Attempted to kill init!");

        zap_pid_ns_process(pid_ns);
        write_lock_irq(&tasklist_lock);
        pid_ns->child_reaper = init_pid_ns.child_reaper;
    }
    return pid_ns->child_reaper;
}

/*
 * 遍历所有子进程并成为他们设置新的父进程
 *
 * */
reaper = find_new_reaper(father);
list_for_each_entry_safe(p, n, &father->child_reaper, sibling) {
    p->real_parent = reaper;
    if(p_parent == father) {
        BUG_ON(p->ptrace);
        p->parent = p->real_parent;
    }
    reparent_thread(p, father);
}

/**
 * 然后调用ptrace_exit_finish()同样进行新的寻父过程，不过这次是给ptraced的子进程寻找父亲
 *
 */
void exit_ptrace(struct task_struct *tracer)
{
   struct task_struct *p, *n;
    LIST_HEAD(ptrace_dead);
    
    write_lock_irq(&tasklist_lock);
    list_for_each_entry_safe(p, n, &tracer->ptraced, ptrace_entry) {
        if(__ptrace_detach(tracer, p))
            list_add(&p->ptrace_entry, &ptrace_dead);
    }
    write_unlock_irq(&tasklist_lock);
    BUG_ON(!list_empty(&tracer->ptraced));

    list_for_each_safe(p, n, &ptrace_dead, ptrace_entry) {
        list_del_init(&p->ptrace_entry);
        release_task(p);
    }
}
