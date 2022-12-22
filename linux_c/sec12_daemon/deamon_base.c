#if 0
    1 守护进程特征:
            ps -a(其他用户拥有的)x(没有控制终端的)j(显示作业相关的如会话ID，进程组ID等)
        父进程ID为0的各进程通常是内核进程，其作为系统引导装入过程的一部分而启动（init是个例外，
    它是由内核在引导装入时启动的用户层次的命令）。内核进程通常存在于系统的整个生命期中，
    以超级用户特权运行，无控制终端，无命令行。
        内核守护进程的名字出现在方括号中，常见的内核守护进程：
    kswapd: 内存换页守护进程，支持虚拟内存子系统在经过一段时间后将脏页数据慢慢的写回磁盘来回收这些页面
    flush: 在可用内存达到设置的最小阈值时将脏页面冲洗至磁盘。每个写回设备都有一个冲洗守护进程
    sync_supers:定期激昂文件系统元数据冲洗至磁盘
    jbd: 帮助实现ext4文件系统中的日志功能
    init: 进程号为1，系统守护进程，主要负责启动各运行层次特定的系统服务
    rpcbind: 提供将远程过程调用程序号映射为网络端口号的服务
    inetd: 侦听网络接口，以便取得来自网络的对各种网络服务进程的请求
    cron: 在定期安排的日期和时间执行命令


    2 编程规则：
        （1）调用umask将文件模式创建屏蔽字设置为一个已知值(通常是0)
        （2）调用fork，然后使父进程exit
        （3）调用setsid创建一个新会话，
        （4）将当前工作目录更改为根目录
        （5）关闭不再需要的文件描述符
        （6）某些守护进程打开/dev/null使其具有文件描述符0,1和2

    3 出错记录：
        （1）产生日志消息的方法：
            a. 内核例程可以调用log函数，任何一个用户进程可以通过打开并读取/dev/klog 设备来读取这些消息
            b. 大多数用户进程(守护进程)调用syslog(3)函数来产生日志消息，这使消息将被发送至UNIX域数据报
                套接字/dev/log
            c. 无论一个用户进程是在此主机上，还是通过TCP/IP网络连接到此主机的其他主机上，都可将日志消息法相UDP端口514,

        （2）syslogd守护进程读取所有3种格式的日志消息，此守护进程启动时读一个配置文件，其文件名一般为/etc/syslog.conf，
            该文件决定了不同种类的消息应送向何处。

            void openlog(const char *ident, int option, int facility);
            void syslog(int priority, const char *format, ...);
            void closelog(void);
            int setlogmask(int maskpri);
    4 单实例守护进程：
            为了正常运作，某些守护进程实现为，在任意时刻只运行该守护进程的一个副本，例如，这种进程可能需要排他地访问一个设备。
            如果守护进程需要访问一个设备，而该设备驱动程序有时会阻止想要多次打开/dev目录下相应设备节点的尝试。这就限制了在一
        个时刻只能运行守护进程的一个副本，但是如果没有这种设备可供使用，那么我们需要自行处理。
            文件和记录锁机制为一种方法提供了基础，该方法保证一个守护进程只有一个副本在运行。如果每一个守护进程创建一个有固有
        名字的文件，并在该文件的整体上加一把写锁，那么只允许创建一把这样的写锁，在此之后创建写锁的尝试都会失败，这向后续守护
        进程副本指明已有一个副本正在运行，文件和记录锁提供了一种方便的互斥机制，如果守护进程在一个文件的整体上得到一把写锁，
        那么在该守护进程终止时，这把锁将被自动删除，这就简化了复原所需的处理，去除了对以前的守护进程实例需要进行清理的有关操作

#endif

/* demo：利用文件和记录锁保证一个时刻只能有一个守护进程副本运行 */
#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

extern int lockfile(int);

int already_running(void)
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if(fd < 0) {
        syslog(LOG_ERR, "cannot open %s : %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    if(lockfile(fd) < 0) {
        if(errno == EACCESS || errno == EAGAIN) {
            close(fd);
            return (1);
        }
        syslog(LOG_ERR, "cannot lock %s : %s ", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return (0);
}


void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    
    /* cleat file creation mask */
    umask(0);

    /* get maxmum number of file descriptors */
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        fprintf(stderr, "%s :cannot get file limit \r\n", cmd);
        exit(-1);
    }

    /* become a session leader to lose controlling TTY */
    if((pid = fork()) < 0) {
        fprintf(stderr, "%s :cannot fork \r\n", cmd);
        exit(-1);
    }else if(pid != 0) { /* parent */
        exit(0);
    }
    setsid();

    /* ensure future opens won't allocate controlling TTYs */
    sa.sa_handler =  SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sas, NULL) < 0) {
        fprintf(stderr, "%s :cannot ignore SIGHUP \r\n", cmd);
        exit(-1);
    }
    if((pid = for()) < 0) {
        fprintf(stderr, "%s :cannot fork \r\n", cmd);
        exit(-1);
    } else if(pid != 0) { /* parent */
        exit(0);
    }
    
    /* change the current working directroy to the root so we won't 
     * prevent file systems from being unmounted
     * */
    if(chdir("/") < 0) {
        fprintf(stderr, "%s : cannot change directory to root \r\n", cmd);
        exit(-1);
    }
    
    /* close all open file descriptors */
    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for(i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    /* attach file descriptors 0, 1, 2 to /dev/null  */
    fd0 = open("dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    /* initialize the log file */
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d \r\n",
                fd0, fd1, fd2);
        exit(1);
    }
}
