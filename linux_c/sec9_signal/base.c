#if 0
    1 信号概念：
        a. 产生条件：
            （1）终端按键：Ctrl+C->SIGINT
            （2）硬件异常：除数为0，无效内存引用，
            （3）调用kill函数
            （4）kill命令：kill函数的接口
            （5）软件条件的发生：如SIGURG（网络连接上传来带外数据）,SIGPIPE（管道读进程终止，进程写管道）,
                SIGALRM（设置的闹钟时钟超时）
        b. 信号处理：信号是异步事件，是软件中断，软件层次上对中断机制的一种模拟(依赖于中断)，需提前告诉内核信号出现时的处理情况
            （1）忽略信号：大多数都可忽略，SIGKILL和SIGSTOP除外，原因：其向超级用户提供了使进程终止或停止的可靠方法
            （2）捕捉信号：SIGCHILD,SIGTERM...
            （3）系统默认动作

    2 signal函数
        a. void (*signal(int signo, void (*func)(int)))(int)  （demo_signal_func）
                    ||
                    \/
            typedef void (*sighandler_t)(int)
            sighandler_t signal(int signum, sighandler_t handler);
            参数：signo：信号 func:SIG_IGN(忽略)， SIG_DFL(默认处理)或处理函数地址
            返回值：signal以前的处理配置，出错返回SIG_ERR
            注意：注意全局变量的gcc优化   
    3 程序启动：
            a. 执行一个程序时，所有信号的状态都是默认或者忽略
            b. exec函数会将原先设置的要捕捉的信号都更改为默认动作，其他信号状态不变(捕捉函数地址将无效)
            c. 后台进程对中断和退出信号处理为忽略
            d. fork函数，子进程继承父进程的信号处理方式（子进程复制了父进程的存储映像，信号捕捉函数的地址在进程中是有意义的）


    4 不可靠信号：信号的丢失和不确定性
    
    5 信号响应过程：mask和pending Bit
    
    6 中断的系统调用
        a. 低速系统调用和其他系统调用（阻塞和非阻塞）：
        b. 中断系统调用：进程正在执行一个低速系统调用，阻塞期间捕捉到一个信号，该系统调用将被打断不再执行(被唤醒)，返回出错，并将
            errno设为EINTER
            while((len = read(fd, buf, BUFFSIZE)) < 0) {
                if(errno == EINTER)
                    /* goto again */
                    continue;
                /* handler other errnors */
            }
        c. select 和 poll
    
    7 可重入函数
        a. 不可重入：
            （1）使用静态数据结构
            （2）调用malloc和free
            （3）标准IO函数，标准IO库很多实现都以不可重入方式使用全局数据结构
        b. 信号处理程序中不能出现不可重入函数的原因：由于信号的不可预知性，信号处理程序过程中捕捉到同一个信号时，此时会产生返回给
            正常调用者的信息可能被返回给信号处理程序的信息覆盖
        c. 信号处理程序中避免出现不可重入函数，其结果是不可预见的(no_reentrancy_test())
    
    8 可靠信号术语和语义
        
    9 kill和raise函数
        int kill(pid_t pid, int signo);
        int raise(int signo);

    10 alarm 和 pause函数
        unsigned int alarm(unsigned int seconds);
        int pause(); 返回-1, 并将errno 设置为EINTR,注：执行了信号处理程序并从其返回时pause才返回
        a. sleep实现和考虑(sleep1(), sleep2(), sleep3(), sleep4())
        b. 超时限制的io调用(demo_read_timeout)
        

#endif
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <setjmp.h>
#include <sys/wait.h>


static volatile int value = 0;
static jmp_buf env_alrm;

static void user_handle(int signum)
{
    value++;
    if(signum == SIGUSR1)
        printf("received signal SIGUSR1 %d \r\n", SIGUSR1);
    else if(signum == SIGUSR2)
        printf("received signal SIGUSR2 %d \r\n", SIGUSR2);
}

static void sigalrm_handler(int signum)
{
    struct passwd *rootptr;
    alarm(1);
    if((rootptr = getpwnam("root")) == NULL)
        fprintf(stderr, "getpwname \"root \" failed \r\n");
}

static int no_reentrancy_test()
{
    int ret = 0;
    struct passwd *ptr;
    signal(SIGALRM, sigalrm_handler);
    alarm(1);
    for(;;) {
        if((ptr = getpwnam("wangzl")) == NULL) {
            fprintf(stderr, "getpwdname \"wangzl\" failed \r\n");
            ret = -1;
            break;
        }
        if(strcmp(ptr->pw_name, "wangzl") != 0) {
            fprintf(stderr, "warnning : return value corrupteed, pwname = %s \r\n", ptr->pw_name);
            ret = -1;
            break;
        }
        printf("getpwdnam(\"wangzl\") = %s \r\n", ptr->pw_name);
    }
    return ret;
}

static void sig_alrm(int signum)
{
    /* nothing to do, just return to wake up the pause */
}

/*注意几个问题：
 * （1）alarm之前已经设置闹钟值-> a.返回值小于参数，等待闹钟超时；b.大于此参数，在sleep1返回之前复位此闹钟
 * （2）SLALRM配置之前已经被更改，需要保存返回值，返回前恢复
 * （3）第一次调用alarm和调用pause之间有一个竞争条件，繁忙的系统中，可能alarm在调用pause之前超时并调用信号处理程序，此时如果没有
 *      捕获到其他信号，调用者将被永远挂起
 *
 * */
static int sleep1(unsigned int nsec)
{
    if(signal(SIGALRM, sig_alrm) == SIG_ERR)
        return (nsec);
    alarm(nsec);
    pause();
    return alarm(0);
}

/* 问题(1)解决*/
static int sleep2(unsigned int nsec)
{
    unsigned int prev_sec;
    if(signal(SIGALRM, sig_alrm) == SIG_ERR)
        return (nsec);
    prev_sec = alarm(nsec);
    if(prev_sec > 0 && prev_sec < nsec) {
        alarm(prev_sec);
        pause();                /* wait last timer out */
        alarm(nsec-prev_sec);       /*reararm for remainder time*/
    }
    pause();                    /* our timer out */
    if(prev_sec > nsec) {       /* reserve other timer */
        alarm(prev_sec - nsec);
    }
}
/* 问题(2)解决 */
static int sleep3(unsigned int nsec)
{
    unsigned int prev_sec;
    void (*prev_handler)(int);
    if((prev_handler = signal(SIGALRM, sig_alrm)) == SIG_ERR)
        return (nsec);
    prev_sec = alarm(nsec);
    if(prev_sec > 0 && prev_sec < nsec) {
        if(prev_handler != SIG_DFL && signal(SIGALRM, prev_handler) == SIG_ERR)        /* reserve last handler */
            fprintf(stderr, "Warnning: reserved sigalrm handler failed \r\b");
        alarm(prev_sec);
        pause();                /* wait last timer out */
        if(signal(SIGALRM, sig_alrm) == SIG_ERR)        /* reserve last handler */
            fprintf(stderr, "Warnning: reserved sigalrm handler failed \r\b");
        alarm(nsec-prev_sec);       /*reararm for remainder time*/
    }
    pause();                    /* our timer out */
    if(prev_handler != SIG_DFL && signal(SIGALRM, prev_handler) == SIG_ERR)        /* reserve last handler */
        fprintf(stderr, "Warnning: reserved sigalrm handler failed \r\b");
    if(prev_sec > nsec) {       /* reserve other timer */
        alarm(prev_sec - nsec);
    }
}

/*
 * 问题3解决策略,使用setjmp和longjmp
 *  考虑隐藏问题：如果SIGALRM中断其他信号处理程序，longjmp将会提前结束它
 * */
static void sig_alrmjmp(int signum)
{
    printf("%d -> SINALRM received \r\n", getpid());
    longjmp(env_alrm, 1);   //jump with no pause
}

static int sleep4(unsigned int nsec)
{
    unsigned int prev_sec;
    void (*prev_handler)(int);
    if((prev_handler = signal(SIGALRM, sig_alrmjmp)) == SIG_ERR)
        return (nsec);
    prev_sec = alarm(nsec);
    if(prev_sec > 0 && prev_sec < nsec) {
        if(prev_handler != SIG_DFL && signal(SIGALRM, prev_handler) == SIG_ERR)        /* reserve last handler */
            fprintf(stderr, "Warnning: reserved sigalrm handler failed \r\b");
        alarm(prev_sec);
        pause();                /* wait last timer out */
        if(signal(SIGALRM, sig_alrmjmp) == SIG_ERR)        /* reserve last handler */
            fprintf(stderr, "Warnning: reserved sigalrm handler failed \r\b");
        alarm(nsec-prev_sec);       /*reararm for remainder time*/
    }
    if(setjmp(env_alrm) == 0)       /* has received SIGALRM, don't pause*/
        pause();                    /* our timer out */
    printf("%d -> timeout \r\n", getpid());
    if(prev_handler != SIG_DFL && signal(SIGALRM, prev_handler) == SIG_ERR)        /* reserve last handler */
        fprintf(stderr, "Warnning: reserved sigalrm handler failed \r\b");
    if(prev_sec > nsec) {       /* reserve other timer */
        alarm(prev_sec - nsec);
    }
}

/*
 * 另一种方式，设置信屏蔽字与原子操作
 * */
static void sigalrm_sleep_handler(int signum)
{
    /* nothing to do . just return */
}

static unsigned int sleep5(unsigned int nseconds)
{
    struct sigaction act, oldact;
    sigset_t newset, oldset, suspendset;
    unsigned int unslept;
    
    if(sigemptyset(&newset) < 0 ) {
        fprintf(stderr, "failed sigemptyset \r\n");
        return 0;
    }
    if(sigaddset(&newset, SIGALRM) < 0) {
        fprintf(stderr, "failed sigaddset \r\n");
        return 0;
    }
    sigemptyset(&act.sa_mask);
    act.sa_handler = sigalrm_sleep_handler;
    act.sa_flags = 0;
    if(sigaction(SIGALRM, &act, &oldact) < 0) {
        fprintf(stderr, "failed to sigaction \r\n");
        return 0;
    }
    sigprocmask(SIG_BLOCK, &newset, &oldset);
    suspendset = oldset;
    sigdelset(&suspendset, SIGALRM);
    alarm(nseconds);
    sigsuspend(&suspendset);

    unslept = alarm(0);
    sigaction(SIGALRM, &oldact, NULL);

    sigprocmask(SIG_SETMASK, &oldset, NULL);
    return unslept;
}


static void sigalrm_handle_uncorrect(int signum)
{
    printf("sigalrm received \r\n");
}
/*
 * 考虑两个问题：
 * （1）在alarm(sec)和read间存在竞争条件，可能存在时钟闹钟时间到达还未调度到read
 * （2）当前系统调用是自动重启动的（例如当前从标准输入读），则从SIGALRM信号处理
 *      程序返回时，此系统调用将不会被中断，而会继续执行
 *  解决方案：
 *      采用setjmp和longjmp
 * */

#define MAXLINE (2048)
static int demo_read_timeout_uncorrect(unsigned int sec)
{
    int n;
    char line[MAXLINE];
    
    if(signal(SIGALRM, sigalrm_handle_uncorrect) == SIG_ERR) {
        fprintf(stderr, "signal SIGALRM failed \r\n");
        return -1;
    }
    alarm(sec);
    if((n = read(STDIN_FILENO, line, MAXLINE)) < 0) {
        fprintf(stderr, "read from stdin failed \r\n");
        return -1;
    }
    alarm(0);
    write(STDOUT_FILENO, line, n);
    return 0;
}

static jmp_buf jmpenv;

static void sigalrm_handle_correct(int signum)
{
    longjmp(jmpenv, 1);
}

static int demo_read_timeout_correct(int sec)
{
    int n;
    char line[MAXLINE];
    
    if(signal(SIGALRM, sigalrm_handle_correct) == SIG_ERR) {
        fprintf(stderr, "resiger SIGALRM failed \r\n");
        return -1;
    }
    if(setjmp(jmpenv) != 0) {
        printf("read timeout ! \r\n");
        return -1;
    }
    alarm(sec);
    if((n = read(STDIN_FILENO, line, MAXLINE)) < 0) {
        fprintf(stderr, "read from stdin failed \r\n");
        return -1;
    }
    alarm(0);
    write(STDOUT_FILENO, line, n);
    return 0;
}


static void sig_interrupt_handler(int signum)
{
    int i, j;
    printf("%d->SIGINT received \r\n", getpid());
    for(i = 0; i <300000; i++)      /* longer than 2 seconds */
        for(j = 0; j < 4000; j++)
            value++;
}

int main()
{
#if 0
    if(signal(SIGUSR1, user_handle) == SIG_ERR)
        fprintf(stderr, "register SIGUSR1 failed \r\n");
    if(signal(SIGUSR2, user_handle) == SIG_ERR)
        fprintf(stderr, "register SIGUSR2 failed \r\n");
    while(1) {
        pause();
        printf("value = %d \r\n", value);
    }
    no_reentrancy_test();
    sleep1(2);
    alarm(1);
    sleep3(2);
#endif
#if 0
    /* sleep4 test start*/
    pid_t pid;
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
    } else if(pid == 0) {       /* 注：例程不合理，不确定调度顺序，需要IPC操作 */
        printf("child : %d \r\n", getpid());
        if(signal(SIGINT, sig_interrupt_handler) == SIG_ERR)
            fprintf(stderr, "Warnning : signal SIGINT failed \r\n");
        sleep4(2);
        printf("%d -> value = %d \r\n", getpid(), value);
    } else {
        printf("parent : %d \r\n", getpid());
        sleep4(1);
        printf("signal SIGINT to %d \r\n", pid);
        kill(pid, SIGINT);
        waitpid(pid, NULL, 0);
        printf("%d -> value = %d \r\n", getpid(), value);
    }
    /* sleep4 test end */
#endif
    //demo_read_timeout_uncorrect(6);
  //  demo_read_timeout_correct(6);
    printf("%d \r\n", sleep5(5));
    exit(0);
}
