#if 0
    同步属性
        (1)互斥量属性 pthread_mutexattr_t
        
        int pthread_mutexattr_init(pthread_mutexattr_t *attr);
        int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

            a. 进程共享：
                int pthread_mutexattr_getshared(const pthread_mutexattr_t *restrict attr, int *restrict pshared);
                int pthread_mutexattr_setshared(pthread_mutexattr_t *attr, int pshared);

                @pshared : PTHREAD_PROCESS_PRIVATE, PTHREAD_PROCESS_SHARED

            b. 健壮属性：多个进程间的互斥量共享有关，当持有互斥量的进程终止时，需要解决互斥量状态恢复的问题。
                这种情况发生时，互斥量处于锁定状态，恢复起来困难，其他阻塞在这个锁的进程将会一直阻塞下去。

                int pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict attr, int *restrict robust);
                int pthread_mutexattr_setrobust(const pthread_mutexattr_t *restrict attr, int robust);

                @robust: PTHREAD_MUTEX_STALLED(默认)，持有互斥量的进程终止时不需要采取别的动作
                        PTHREAD_MUTEX_ROBUST:将导致线程调用pthread_mutex_lock 获取锁，而该锁被另一个进程持有，
                            但它终止时没有对该锁解锁，此时线程会阻塞，从pthread_mutex_lock返回的值为EOWNERRAD而不是0
                            应用程序可通过判断返回值来恢复互斥量

                            恢复互斥量：int pthread_mutex_consistent(pthread_mutex_t *mutex);
          c. 类型属性
                PTHREAD_MUTEX_NORMAL :标准属性，不做特殊错误检查或死锁检测
                PTHREAD_MUTEX_ERRORCHECK :提供错误检查
                PTHREAD_MUTEX_RECURSIVE :允许同一线程对同一互斥量多次加锁，递归互斥量计数，加锁数和解锁数不相等时，将处于加锁状态
                PTHREAD_MUTEX_DEFAULT

                int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr, int *restrict type);
                int pthread_mutexattr_settype(const pthread_mutexattr_t *restrict attr, int restrict type);
        (2)读写锁属性: pthread_rwlockattr_t
            int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
            int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
            进程共享:
            int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *restrict attr, int *restrict pshared);
            int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

        (3)条件变量属性：pthread_condattr_t
            int pthread_condattr_init(pthread_condattr_t *attr);
            int pthread_condattr_destroy(pthread_condattr_t *attr);
            进程共享：
            int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr, int *restrict pshared);
            int pthread_condattr_setpshare(pthread_condattr_t *attr, int pshared);

            时钟属性：设置pthread_cond_timedwait使用的时钟ID
            int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t *clock_id);
            int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);

        (4)屏障属性：pthread_barrierattr_t 
            int pthread_barrierattr_init(pthread_barrierattr_t *attr);
            int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
            进程共享
            int pthread_barrierattr_getpshared(const pthread_barrierattr_t *restrict attr, int *restrict pshared);
            int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);


#endif
