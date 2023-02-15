#if 0
    进程创建：fork通过拷贝当前进程创建一个子进程，区别仅仅在于PID,PPID,和某些资源和统计量, exec函数负责读取可执行
        文件并将其载入地址空间开始运行。

    1 写时拷贝（copy-on-write）

    2 fork():
        fork(),vfork(),__clone()->clone(int (*fn)(void *), void *child_stack, int flags, void *arg, .../* pid_t *ptid, struct user_desc *tls, pid_t *ctid */)
                                        ->do_fork()
                                                ->copy_process();
        copy_process: 
            (1) 调用dup_task_struct()为新进程创建一个内核栈，thread_info结构和task_struct,此时与当前进程完全相同
            (2) 检查用户拥有进程数有无超出限制
            (3) 进程描述符成员清零或设为初始值，不是继承来的进程描述符成员，主要是统计信息，大多数数据依然未被修改
            (4) 子进程状态设置为TASK_UNINTERRUPTIBLE.保证不会投入运行
            (5) 调用copy_flags()更新task_struct的flags成员，PF_SUPERRIV标志清零，PF_FORKNOEXEC标志被设置
            (6) 调用alloc_pid()为新进程分配有效的PID
            (7) 根据传递给clone的参数标志，copy_process()拷贝或共享打开的文件，文件系统信息，信号处理函数，进程地址空间和命名空间等。
            (8) 做扫尾工作并返回一个指向子进程的指针
        返回do_fork函数，如果copy_process函数成功返回，新创建的子进程被唤醒并让其投入运行。

        3 vfork():
            除了不拷贝父进程的页表项外，vfork()系统调用和fork()的功能相同。子进程作为父进程的一个单独的线程在它的地址空间运行，
            父进程被阻塞，直到子进程退出或执行exec，子进程不能向地址空间写入。
#endif 
