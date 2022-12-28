#if 0
    1 记录锁：当第一个进程正在读或修改文件的某个部分时，使用记录锁可以阻止其他进程修改同一文件区（字节范围锁）。
    （1）fcntl记录锁
        int fcntl(int fd, int cmd, ... /* struct flock* */)
        @cmd : F_GETLK, F_SETLK, F_SETLKW
        struct flock {
            short l_type;   /* F_RDLCK(共享读锁), F_WRLCK（独占性写锁）, or F_UNLCK（解锁） */
            short l_whence; /* SEEK_SET, SEEK_CUR, or SEEK_END */
            off_t l_start;  /* starting offset for lock */
            off_t l_len;    /* number of bytes to lock */
            pid_t l_pid;    /* PID of process blocking our lock (only F_GETLK) */
        }
        关于加锁解锁区域：
            a. 指定区域起始偏移量的两个元素与lseek函数中最后两个参数类似，l_whence
                可选用的值是SEEK_SET, SEEK_CUR或SEEK_END
            b. 锁可以在当前文件尾端处开始或者越过尾端处开始，但是不能在文件起始位置之前开始。
            c. 如若l_len为0，则表示锁的范围可以扩展到最大可能偏移量，这意味着不管向该文件中
                追加写了多少数据，他们都可以处于锁的范围内，而且起始位置可以是文件中的任意
                一个位置。
            d. 为了对整个文件加锁，我们设置l_start 和l_whence指向文件起始位置，并且指定长度
                l_len 为0，
        
        加锁规则：
            a. 任意多个进程在一个给定的字节上可以有一把共享的读锁，但是在一个给定的字节上只能有一个进程
            有一把独占写锁。进一步而言，如果在一个给定字节上已经有一把或多把读锁，则不能在该字节上再加写锁；
            如果在一个字节上已经有一把独占性写锁，则不能在对它加任何锁。
            b. 如果单个进程对一个文件区间已经有了一把锁，后来该进程又企图在同一文件区间在加一把锁，那么新
            锁将替换已有锁。
            c. 加读锁时，该描述符必须是读打开。加写锁时，该描述符必须是写打开。

        fcntl三种命令：
            a. F_GETLK: 判断由flock描述的锁是否会被另外一把锁排斥（阻塞）。如果存在一把锁，它阻止创建由flock所描述的
            锁，则该现有锁的信息将重写flock指向的信息。如果不存在这种情况，则除了将l_type设置为F_UNLCK之外，flock所指向
            结构中的其他信息保持不变。
            b. F_SETLK: 设置由flock所描述的锁，如果我们试图获得一把读锁（l_type为F_RDLCK）或写锁(l_type为F_WRLCK)，而兼
            容性规则阻止系统给我们这把锁，那么fcntl会立即出错返回，此时errno设置为EACCESS或EAGAIN
            c. F_SETLKW: 这个命令是F_SETLK的阻塞版本（W表示wait）。
        
        死锁：
            一个进程已经控制了文件的某一区域，此时由试图对另一个进程控制的区域加锁，此时可能会出现死锁
            解决策略：采用IPC实现同步
    
        锁的隐含继承和释放：
            a. 锁与进程和文件两者相关联，当一个进程终止时，它所建立的锁全部释放；无论一个描述符何时关闭，该进程通过
            这一描述符引用的文件上的任何一把锁都会释放。
                    fd1 = open(pathname, ...);
                    read_lock(fd1, ...);
                    fd2 = dup(fd1);
                    close(fd2);
            在close(fd2)后，在fd1上设置的锁会被释放。
            b. 由fork产生的子进程不继承父进程所设置的索。
            c. 执行exec后，新程序可以继承原执行程序的锁（如果对一个文件描述符设置了执行关闭时标志，那么当作为exec
            的一部分关闭该文件描述符时，将释放相应文件所有锁）
#endif

/* 封装fcntl,避免每次分配flock结构 */
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return (fcntl(fd, cmd, &lock));
}

/* 加锁或解锁一个文件区域，使用宏概括 */
#define read_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_UNLCK, (offset), (whence), (len))

/* 测试一把锁 */
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    if(fcntl(fd, F_GETLK, &lock) < 0) {
        fprintf(stderr, "fcntl failed \r\n");
        return 0;
    }
    if(lock.l_type == F_UNLCK)  
        return (0);     /* false, region isn't locked by another proc */

    return (lock.l_pid);    /* true, return pid of lock owner */
}

/* 锁判断 */
#define is_read_lockable(fd, offset, whence, len)   \
    (lock_test(fd, F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len)  \
    (lock_test(fd, F_WRLCK, (offset), (whence), (len)) == 0)
//注意：不能测试进程自己是否在文件的某一部分持有一把锁。

/* 死锁避免：同步 */
static void lockabyte(const char *name, int fd, off_t offset)
{
    if(writew_lock(fd, offset, SEEK_SET, 1) < 0) {
        fprintf(stderr, "failed to write_lock %s \r\n", name);
        return;
    }
    printf("%s : got lock ,byte %lld \r\n", name, (long long) offset);
}

int main()
{
    int fd;
    pid_t pid;
    int ret = 0;
    const char *file_name = "tmplock";

    /* create file and write two bytes to it */
    if((fd = create(file_name ,FILE_MODE)) < 0) {
        fprintf(stderr, "create file %s failed \r\n", file_name);
        exit(1);
    }
    if(write(fd, "ab", 2) != 2) {
        fprintf(stderr, "fail to write to %s \r\n", file_name);
        ret = -1;
        goto close_tmplock;
    }
    if((pid = fork()) < 0) {
        fprintf(stderr, "failed to fork \r\n");
        ret = -1;
        goto close_tmplock;
    } else if(pid == 0) {
        lockabyte(file_name, fd, 0);
        TELL_PARENT(getppid());       
        WAIT_PARENT();
        lockabyte(file_name, fd, 1);
    } else {
        lockabyte(file_name, fd, 1);
        TELL_CHILD(pid);
        WAIT_CHILD();
        lockabyte(file_name, fd, 0);
    }

close_tmplock:
    close(fd);
    exit(ret);
}
