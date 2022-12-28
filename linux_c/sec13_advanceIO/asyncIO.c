#if 0
    1 异步IO（POSIX标准）：
        异步接口使用AIO控制块（aiocb结构）来描述IO操作。
        struct aiocb {
            int aio_fildes;     /* file descriptor */
            off_t aio_offset;   /* file offset for I/O */
            volatile void *aio_buf;     /* buffer for I/O */
            size_t aio_nbytes;      /* numer of bytes to transfer */
            int aio_reqprio;        /* priority */
            struct sigevent aio_sigevent;   /* signal information */
            int aio_lio_opcode;     /* operation for list I/O */
        };
        struct sigevent {
            int sigev_notify;       /* notify type */
            int sigev_signo;        /* signal number */
            union sigval sigev_value;   /* notify argument */
            void (*sigev_notify_function) (union sigval);   /* notify function */
            pthread_attr_t *sigev_notify_attribute;     /* notify attrs */
        };
        @sigev_notify:
        a. SIGEV_NONE : 异步IO请求完成后，不通知进程
        b. SIGEV_SIGNAL : 异步IO请求完成后，产生由sigev_signo字段指定的信号
        c. SIGEV_THREAD : 异步IO请求完成后，由sigev_notify_function字段指定的函数被调用。
        sigev_value 字段被传入作为它的唯一参数。

    2 异步读写：
        int aio_read(struct aiocb *aiocb);
        int aio_write(struct aiocb *aiocb);
    强制等待所有等待中的异步操作不等待而写入持久化存储中，可以独立一个AIO控制块并调用aio_fsync函数
    int aio_fsync(int op, struct aiocb *aiocb);
    @op: O_DSYNC(相当于fdatasync),O_SYNC(相当于fsync)
    
    获得一个异步读，写或者同步操作的完成状态：
        int aio_error(const struct aiocb *aiocb);
        @return : a. 0 : 异步操作成功完成
                  b. -1 : 对aio_error调用失败，errno被设置
                  c. EINPROGRESS: 其他任何返回值是相关的异步操作失败返回的错误码
    如果异步操作成功，可以调用aio_return 函数来获取异步操作的返回值
    ssize_t aio_return(const struct aiocb *aiocb);
    注意：不能提前调用，并且只能调用一次
    
    执行IO操作时，如果还有其他事务要处理而不想被IO操作阻塞，就可以使用异步IO，然而，如果在完成了所有事务时，
    还有异步操作未完成时，可以调用aio_suspend函数来阻塞进程，直到操作完成
    int aio_suspend(const struct aiocb *const aiocb_list[], int nitems, const struct timespec *timeout);
#endif
