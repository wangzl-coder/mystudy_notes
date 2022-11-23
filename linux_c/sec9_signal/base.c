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

    
#endif
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile int value = 0;

static void user_handle(int signum)
{
    value++;
    if(signum == SIGUSR1)
        printf("received signal SIGUSR1 %d \r\n", SIGUSR1);
    else if(signum == SIGUSR2)
        printf("received signal SIGUSR2 %d \r\n", SIGUSR2);
}

int main()
{   
    if(signal(SIGUSR1, user_handle) == SIG_ERR)
        fprintf(stderr, "register SIGUSR1 failed \r\n");
    if(signal(SIGUSR2, user_handle) == SIG_ERR)
        fprintf(stderr, "register SIGUSR2 failed \r\n");
    while(1) {
        pause();
        printf("value = %d \r\n", value);
    }
    exit(0);
}
