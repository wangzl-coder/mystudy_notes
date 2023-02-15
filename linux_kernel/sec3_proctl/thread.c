#if 0
    线程在Linux中的实现：同一程序内共享内存地址空间运行，还共享打开的文件和其他资源，Linux把所有的线程当作进程来实现，
        仅仅被视为一个与其他进程共享某些资源的进程。每个线程都拥有唯一隶属于自己的task_struct,

    创建线程：
        clone(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, 0)
     clone 标志：

     内核线程：独立运行在内核空间的标准进程，没有独立的地址空间，只在内核空间运行，可以被调度和抢占
        创建内核线程：
            struct task_struct *kthread_create(int (*threadfn)(void *data), void *data, const char namefmt[], ...);
            @threadfn: 线程执行函数,
            @data: 给threadfn的传参
            @namefmt：进程命名
        新创建的线程处于不可运行状态，可通过wake_up_process唤醒，
        或者通过kthread_run创建线程并使其运行：
            struct task_struct *kthread_run(int (*threadfn)(void *data), void *data, const char namefmt[], ...)
            宏定义实现，只是调用thread_create和wake_up_process

        #define kthread_run(threadfn, data, namefmt, ...)                   \
        ({                                                                  \
                struct task_struct *k;                                      \
                k = kthread_create(threadfn, data, namefmt, **__VA_ARGS_);  \
                if(!IS_ERR(k))                                              \
                    wake_up_process(k);                                     \
                k;                                                          \
         })

        退出： 调用do_exit()或者其他线程调用kthread_stop()
            int kthread_stop(struct task_struct *k)

    
#endif
