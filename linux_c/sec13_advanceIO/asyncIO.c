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
    @return : 
        a. -1：被一个信号中断，设置errno为EINTR;超时，errno为EAGAIN
        b. 0：有任何IO操作完成

    当我们不想再完成的等待异步IO操作时，可以尝试使用aio_cancel函数来取消他们
    int aio_cancel(int fd, struct aiocb *aiocb)
#endif

    /* demo :ROT-13算法，阻塞IO */
#define BSZ 4096

unsigned char buf[BSZ]

unsigned char translate(unsigned char c)
{
    if(isalpha(c)) {
        if(c >= 'n')
            c -= 13;
        else if(c >= 'a')
            c += 13;
        else if(c >= 'N')
            c -= 13;
        else 
            c += 13;
    }
    return c;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int ifd, ofd, i, n, nw;

    if(argc ! = 3) {
        fprintf(stderr, "usage :rot13 infile outfile \r\n");
        exit(-1);
    }
    if((ifd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "cannot open %s \r\n", argv[1]);
        exit(ifd);
    }
    if((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_NODE)) < 0) {
        fprintf(stderr, "cannot create %s \r\n", argv[2]);
        ret = ofd;
        goto close_infile;
    }
    while((n = read(ifd, buf, BSZ)) > 0) {
        for(i = 0; i < n; i++) {
            buf[i] = translate(buf[i]);
        }
        if(nw = write(ofd, buf, n) != n) {
            if(nw < 0)
                fprintf(stderr, "write failed \r\n");
            else
                fprintf(stderr, "short write (%d/%d) \r\n", nw, n);
            ret = nw;
            break;
        }
    }
    fsync(ofd);
    
    close(ofd);
close_infile:
    close(ifd);
    exit(ret);
}

/* 异步IO实现 */
#define BSZ 4096
#define NBUF 8

enum rwop {
    UNUSED = 0,
    READ_PENDING = 1,
    WRITE_PENDING = 2
}

struct buf {
    enum rwop op;
    int last;
    struct aiocb aiocb;
    unsigned char data[BSZ];
};

struct buf bufs[NBUF];

unsigned char translate(unsigned char c)
{
    if(isalpha(c)) {
        if(c >= 'n')
            c -= 13;
        else if(c >= 'a')
            c += 13;
        else if(c >= 'N')
            c -= 13;
        else
            c += 13;
    }
    return c;
}

int main(int argc, char *argv[])
{
    int ifd, ofd, i, j, n, err, numop;
    struct stat sbuf;
    const struct aiocb *aiolist[NBUF];
    off_t off = 0;

    if(argc != 3)
        err_quit("usage: rot13 infile outfile");
    if((ifd = open(argv[1], O_RDONLY)) < 0)
        err_sys("cannot open %s ", argv[1]);
    if((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
        err_sys("cannot create %s ", argv[2]);
    if(fstat(ifd, &sbuf) < 0)
        err_sys("fstat failed");

    /* initialize the buffers */
    for(i = 0; i < NBUF; i++) {
        bufs[i].op = UNUSED;
        bufs[i].aiocb.aio_buf = bufs[i].data;
        bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
        aiolist[i] = NULL;
    }
    numop = 0;

    for(;;) {
        for(i = 0; i < NBUF; i++) {
            switch(bufs[i].op) {
            case UNUSED:
                /* read from the input file if more data remains unread */
                if(off < sbuf.st_size) {
                    bufs[i].op = READ_PENDING;
                    bufs[i].aiocb.aio_fildes = ifd;
                    bufs[i].aiocb.aio_offset = off;
                    off += BSZ;
                    if(off >= sbuf.st_size)
                        bufs[i].last = 1;
                    bufs[i].aiocb.aio_nbytes = BSZ;
                    if(aio_read(&bufs[i].aiocb) < 0)
                        err_sys("aio_read failed \r\n");
                    aio_list[i] = &bufs[i].aiocb;
                    numop++;
                }
                break;
                
            case READ_PENDING:
                if((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                    continue;
                if(err != 0) {
                    if(err == -1)
                        err_sys("aio_error failed ");
                    else
                        err_exit(err, "read failed");
                }
                /* A read is complete; translate the buffer and write it */
                if((n = aio_return(&bufs[i].aiocb)) < 0)
                    err_sys("aio_return failed ");
                if(n != BSZ && !bufs[i].last)
                    err_quit("short read (%d/%d)", n, BSZ);
                for(j = 0; j < n; j++) {
                    bufs[i].data[j] = translate(bufs[i].data[j])
                }
                bufs[i].op = WRITE_PENDING;
                bufs[i].aiocb.aio_fildes = ofd;
                bufs[i].aiocb.aio_nbytes = n;
                if(aio_write(&bufs[i].aiocb) < 0)
                    err_sys("aio_write failed");
                /* retain out spot in aiolist */
                break;
            
            case WRITE_PENDING:
                if((err = aio_err(&bufs[i].aiocb)) == EINPROGRESS)
                    continue;
                if(err != 0) {
                    if(err == -1)
                        err_sys("aio_write failed");
                    else
                        err_exit(err, "write failed ");
                }
                /* A write is complete ;mark the buffer as unused */
                if((n = aio_return(&bufs[i].aiocb)) < 0)
                    err_sys("aio_return failed ");
                if(n != bufs[i].aiocb.aio_nbytes)
                    err_quit("short write (%d/%d)", n, BSZ);
                aiolist[i] = NULL;
                bufs[i].op = UNUSED;
                numop --;
                break;
            }
        }
        if(numop == 0) {
            if(off >= sbuf.st_size)
                break;
        } else {
            if(aio_suspend(aiolist, NBUF, NULL) < 0)
                err_sys("aio_suspend failed");
        }
    }
    bufs[0].aiocb.aio_fildes = ofd;
    if(aio_fsync(O_SYNC, &bufs[0].aiocb) < 0)
        err_sys("aio_fsync failed ");

    exit(0);
}
