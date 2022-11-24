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

        

#endif
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

static volatile int value = 0;

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
    printf("sigalrm received \r\n");
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
        alarm(prev_sec);
        pause();                /* wait last timer out */
        alarm(nsec-prev_sec);       /*reararm for remainder time*/
    }
    pause();                    /* our timer out */
    if(signal(SIGALRM, prev_handler) == SIG_ERR)        /* reserve last handler */
        fprintf(stderr, "reserved sigalrm handler failed \r\b");
    if(prev_sec > nsec) {       /* reserve other timer */
        alarm(prev_sec - nsec);
    }
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
#endif
//  no_reentrancy_test();
//    sleep1(2);
    alarm(1);
    sleep2(2);
    exit(0);
}
