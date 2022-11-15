#if 0
1 进程描述符
    a. 非负整数表示的唯一进程ID，可重复使用
    b. ID为0的进程为调度进程（也称交换进程swapper），内核部分，系统进程
    c. ID为1的进程init,自举结束后由内核调用，通常读/etc/rc*或/etc/inittab文件，以及/etc/init.d中的文件，
        是普通的用户进程，不会终止。以超级用户权限运行
    d. 
        pid_t getpid(void); //self process id
        pid_t getppid(void); //parent process id
        uid_t getuid(void);     //returns the real user ID of the calling process.
        uid_t geteuid(void);    //returns the effective user ID of the calling process.
        gid_t getgid(void);     //returns the real group ID of the calling process.
        gid_t getegid(void);    //returns the effective group ID of the calling process.

2 fork
    a.  pid_t fork(void);   //return 0 in child process, return child process id in parent process
    b. 系统调用clone
    c. 内存共享与独立，写时拷贝
    d. io缓冲区问题(demo_fork_iobuf)
    e. 文件共享(demo_fork_filshare): fork之前的文件描述符，父子进程共享同一个文件表项，共享同一个文件偏移量，且各自关闭
        fork之后打开同一文件，各自的不同的文件表指向同一个文件i节点，此时不共享文件偏移量，
    f. 子进程继承的其他属性：实际用户，组ID，有效用户，组ID；附加组ID；进程组ID；会话ID；控制终端；设置用户ID标志和设置组ID
        标志；当前工作目录；根目录；文件模式创建屏蔽字；信号屏蔽和安排；针对任一打开文件描述符的在执行时的关闭状态；环境；...
    g. 父子进程区别：fork返回值；进程ID；具有不同的父进程ID；子进程的tms_utime,tms_stime,tms_cutime,以及tms_ustime均设置为0
        父进程设置的文件锁不被子进程继承；子进程的未处理的闹钟alarm被清除；子进程未处理信号集设置为空集..
    h. fork 失败的主要原因：（1）系统中有了太多进程，（2）该实际用户ID的进程总数超过系统限制（CHILD_MAX）
    i. fork两个主要用法：（1）父子进程不同的处理代码，（2）使用exec

3 vfork
    a. pid_t vfork(void);
    b. 与fork区别：（1）不完全复制父进程地址空间，子进程中应立即调用exec或exit （2）vfork保证子进程先运行，在其调用exec或
        exit之后父进程才可能被调度（应避免死锁的发生）
    c. demo_vfork()

4 exit 
    a. 进程5种正常终止方式
        （1）main函数内return，等效于exit
        （2）调用exit(),由ISO C定义，会调用终止处理程序（由钩子函数atexit（）登记），关闭标准IO流等。
        （3）调用_exit或_Exit函数，
        （4）进程的最后一个线程执行返回语句，进程终止状态0
        （5）进程的最后一个线程调用pthread_exit，进程终止状态0,与pthread_exit传参无关，
      三种异常终止方式
        （1）调用abort()，给自己发SIGABRT信号
        （2）当进程收到某些信号时，
        （3）最后一个线程对取消请求做出响应，延迟方式
        
        终止操作：关闭进程所有打开的描述符，释放其存储器
    
    b. 进程状态(ps), 
        R(runnable,on run queue)-->运行:正在运行或在运行队列中等待
        S(sleeping)-->中断:休眠中，受阻，在等待某个条件形成的信号或接收到信号
        D(uninterruptible sleep,usually IO)-->不可中断：收到信号不唤醒和不可运行，进程必须等待直到由中断发生
        Z(Zombie)-->僵死：进程已终止，直到父进程调用wait4()系统调用后释放
        T(traced or stopped)-->停止：进程收到SIGSTOP，SIGSTP，SIGTIN，SIGTOU信号后停止运行
    
    c. 退出状态和终止状态
        子进程将退出状态传参给终止函数（exit，_exit,_Eixt），内核将退出状态转为终止状态，父进程通过wait或waitpid取得

    d. 孤儿进程与进程领养；
        父进程先于子进程终止，子进程成为孤儿进程，此时被init进程领养，大致过程：一个进程终止时，内核检查所有活动进程，判断是否
            是正要终止进程的子进程，如果是，将其父进程ID改为1（init进程ID）。

    e. 僵尸进程：
        子进程先于父进程终止，内核为每个终止子进程保存了一定量的信息（进程ID，终止状态，cpu时间总量），当父进程wait或waitpid时
            可以得到这些信息，内核可以释放终止进程使用的存储区，关闭其所有打开文件。如果子进程终止，父进程没有进行善后处理（wait，
            waitpid），则子进程被称为僵尸进程（Z），应当避免此情况。被init领养的进程不会变成僵尸进程。
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
static void demo_pid()
{
    printf("pid: %d\n",getpid());
    printf("ppid: %d\n",getppid());
    printf("uid: %d\n",getuid());
    printf("euid: %d\n",geteuid());
    printf("gid: %d\n",getgid());
    printf("egid: %d\n",getegid());

}

static int glob = 0;
static const char buf[] = "write to std nobuff\n";
static void demo_fork_iobuf()
{
    int val = 0;
    pid_t pid;
    if(write(STDOUT_FILENO, buf, strlen(buf)) != strlen(buf)){
        fprintf(stderr, "write failed \r\n");
        return;
    }
    fprintf(stdout, "[stdout] fork before       ");
    fprintf(stderr, "[stderr] fork before       ");
    if((pid = fork()) < 0) {
        fprintf(stderr , "fork failed \r\n");
    } else if(pid == 0) {
        glob++, val++;
    } else{
        sleep(2);
    }
    printf("pid = %d, glob = %d, val = %d \r\n", getpid(), glob, val);
    exit(0);
}

static void demo_fork_filshare()
{
    pid_t pid;
    FILE *fp;
    fp = fopen("tmp.file", "w+");
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        return ;
    } 
   // fp = fopen("tmp.file", "w+");   /* a+ */
    if(fp == NULL) {
        perror(NULL);
    }
    if(pid == 0) {
        fputs("this is child process write\n", fp);
    } else {
        sleep(2);
        fputs("this is parent process write\n", fp);
    }
    fclose(fp);
}

static void demo_vfork()
{
    pid_t pid;
    int val = 0;
    FILE *fp;

    fp = fopen("./tmp.file", "w+");
    if(fp == NULL) {
        fprintf(stderr, "fopen failed \r\n");
        return ;
    }
    printf("vfork before!   ");
    if((pid = vfork()) < 0) {
        fprintf(stderr, "vfork failed \r\n");
        return;
    } else if(pid == 0) {
        printf("this is child procecss: pid = %d, glob = %d, val = %d \r\n", getpid(), glob, val);
        glob++, val++;
        fputs("this is child process write \n",fp);
    } else {
        printf("this is parent process: pid = %d, glob = %d, val = %d \r\n", getpid(), glob, val);
        fputs("this is parent process write \n", fp);
        fclose(fp);
    }
    _exit(0);
}

static void demo_fork_exit()
{
    pid_t pid;
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        return ;
    } else if(pid == 0) {
        printf("child process %d pause <--- %d\r\n", getpid(), getppid());
        //pause();
        exit(0);
    }else {
        int status;
        sleep(10);
        //printf("parent process signal \r\n");
        //kill(pid, SIGINT);
        printf("%d wait \r\n", getpid());
        wait(&status);
        printf("status = %d \r\n",status);
    }
}

static void demo_fork_zombie()
{
    pid_t pid;
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        return ;
    } else if(pid == 0) {
        printf("child %d exit <-- %d \r\n", getpid(), getppid());
        exit(0);
    }
    sleep(10);
    printf("parent %d exit \r\n", getpid());
}

static void demo_fork_orphan()
{
    pid_t pid;
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        return ;
    } else if(pid == 0) {
        printf("child %d <-- %d \r\n", getpid(), getppid());
        sleep(10);

        printf("child %d exit <--- %d\r\n", getpid(), getppid());
        exit(0);
    }
    sleep(2);
    printf("parent %d exit<--%d \r\n", getpid(), getppid());
}

int main()
{
//    demo_pid();
//    while(1);
//    demo_fork_iobuf();
//    demo_fork_filshare();
//    demo_vfork();
//    demo_fork_exit();
//    demo_fork_zombie();
    demo_fork_orphan();
    exit(0);
}
