#if 0
    1 自旋锁：与互斥量类似，但它不是通过休眠使进程阻塞，而是在获取锁之间一直处于忙等（自旋）阻塞状态，可用于
        一下情况：锁被持有时间短，而且线程不希望在重新调度上面花费太多成本。
    
    主要作用：用在非抢占式内核的中断处理程序中（Linux属于非抢占式内核），可以防止中断处理程序中出现死锁状态。
        在非抢占式内核中，中断处理程序不能休眠，因此用的同步原语只能是自旋锁

    2 初始化与反初始化
        int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
        int pthread_spin_destroy(pthread_spinlock_t *lock);
        @return : 0 for success or error number
        @pshared : PTHREAD_PROCECSS_SHARED, PTHREAD_PROCESS_PRIVATE

    3 加锁和解锁
        int pthread_spin_lock(pthread_spinlock_t *lock);
        int pthread_spin_trylock(pthread_spinlock_t *lock);
        int pthread_spin_unlock(pthread_spinlock_t *lock);
        @return : 0 for success or error number
        注意：当前锁解锁情况下，加锁不会自旋；已经持有自旋锁的情况下不要调用会引起休眠的函数，这样会浪费CPU资源
            因为其他获取自旋锁的线程等待时间会延长。
#endif
