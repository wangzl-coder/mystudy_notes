#if 0
    1 wait 和waitpid
        a. SIGCHILD信号：子进程终止信号，异步通知，默认动作忽略
        b. 调用wait 和 waitpid时可能的情况：
            （1）其所有子进程都还在运行，则阻塞
            （2）一个子进程已经终止，正等待父进程获取其终止状态，则取得该子进程的终止状态立即返回
            （3）如果它没有子进程，则立即出错返回
            备注：如果进程由于接收到SIGCHLD信号而调用wait，则可期望wait会立即返回，但如果在任意时刻调用wait，则进程可能会阻塞(demo_wait_sigchild)

        c. pid_t wait(int *status);
            pid_t waitpid(pid_t pid, int *status, int options);
            区别：
                （1）子进程终止前，wait使调用者阻塞，而waitpid有options可使调用者不阻塞
                （2）waitpid并不等待在其调用后的第一个终止进程，有若干选项可控制其等待的进程
        
        d. status: 返回状态，整形状态字，获取状态宏(demo_wait_wifstatus)：
            （1）WIFEXITED(status) : true->正常终止，通过WEXITSTATUS(status)取传给exit,_exit,_Exit参数的低8位
            （2）WIFSIGNALED(status) : true->异常终止，通过WTERMSIG(status)取使进程终止的信号编号，
                通过WCOREDUMP(status)可判断是否产生core dump文件
            （3）WIFSTOPPED(status): true->暂停子进程，通过WSTOPSIG(status)取使进程暂停的信号
            （4）WIFCONTINUED(status): true->暂停后已经继续，

        e. pid:
            wait_pid中的pid参数解释
            （1）pid == -1, 等待任一子进程，==wait
            （2）pid > 0: 等待进程ID为pid的子进程
            （3）pid == 0: 等待与调用进程同进程组的任一子进程
            （4）pid < -1: 等待组ID为pid绝对值的任一子进程
        f. options:
            WCONTINUED:
            WNOHANG:
            WUNTRACED:

        g. wait使用技巧：fork一个子进程，为了不想wait等待其结束，也不想让其处于僵死状态，可以使用两次fork,将其交给init领养（demo_wait_secfork()）。

    2 wait3 和 wait4(system call)
        a. pid_t wait3(int *status, int options, struct rusage *rusage);
            pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);
            

    3 竞争条件
        a. 竞争条件：当多个进程对共享数据操作时，预期结果依赖与进程的运行顺序，此时认为发生了竞争条件（demo_test_competition）
        b. 解决策略：轮询进行条件测试，信号，IPC等
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define err_exit(format, arg...)    {   \
    fprintf(stderr, format, ##arg);                 \
    exit(-1);                                       \
}

static void putchar_pertime(const char *str)
{
    const char *ptr;
    for(ptr = str; *ptr != 0; ptr++) {
        putchar(*ptr);
    }
}

static void demo_test_competition()
{
    pid_t pid;
    setbuf(stdout, NULL);       /* set unbuffered */
    if((pid = fork()) < 0)
        err_exit("[%d]: fork failed \r\n", __LINE__)
    else if(pid == 0) {
        putchar_pertime("child process output");
        exit(0);
    } else {
        putchar_pertime("parent process output");
        wait(NULL);
    }
}

static void demo_wait_secfork()
{
    pid_t pid;

    if((pid = fork()) < 0)
        err_exit("[%d] :fork failed \r\n", __LINE__)
    else if(pid == 0) {
        if((pid = fork()) < 0)
            err_exit("[%d] :fork failed \r\n", __LINE__)
        else if(pid > 0) {
            /* first child process. we just exit */
            printf("first child %d exit\r\n", getpid());
            exit(0);
        }
        /* second child process. wait for first exit */
        sleep(2);
        printf("second child process %d exit, parent is %d \r\n", getpid(), getppid());
        exit(0);
    }
    
    /* parent process,we wait it soon */
    if(waitpid(pid, NULL, 0) != pid)
        printf("wait %d failed \r\n", pid);
    printf("first and parent process eixt! \r\n");
}

static void pr_exit(int status)
{
    if(WIFEXITED(status))
        printf("normal termination, exit status is %d \r\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
        printf("abnormal termination, signal number is %d %s \r\n", WTERMSIG(status),
#ifdef WCOREDUMP
        WCOREDUMP(status) ? "core dump file generated" :
#endif
        "");
    else if(WIFSTOPPED(status))
        printf("child stopped, signal num is %d \r\n", WSTOPSIG(status));

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"      /* ignored warnning of devision by zero */
static void demo_wait_wifstatus()
{
    int status;
    pid_t pid;

    /* normal exit */
    if((pid = fork()) < 0)
        err_exit("%d :fork failed",__LINE__)
    else if(pid == 0) {
        printf("child %d exit(7) \r\n", getpid());
        exit(7);
    }
    
    if(wait(&status) != pid)
        err_exit("wait %d failed \r\n", pid)
    pr_exit(status);
    putchar('\n');

    /* exit by abort */
    if((pid = fork()) < 0)
        err_exit("%d :fork failed",__LINE__)
    else if(pid == 0) {
        printf("child %d generates SIGABRT by calling abort()\r\n", getpid());
        abort();
    }

    if(wait(&status) != pid)
        err_exit("wait %d failed \r\n", pid)
    pr_exit(status);
    putchar('\n');

    /* exit by SIGFPE */
    if((pid = fork()) < 0)
        err_exit("%d :fork failed",__LINE__)
    else if(pid == 0) {
        int val = 2;
        printf("child %d generates SIGFPE by devision by zero \r\n", getpid());
        val /=0;
    }

    if(wait(&status) != pid)
        err_exit("wait %d failed \r\n",getpid())
    pr_exit(status);
}
#pragma GCC diagnostic pop


static void sigchild_handler(int signum)
{
    if(signum == SIGCHLD) {
        int status;
        pid_t pid;
        pid = wait(&status);
        printf("%d wait %d status is %d \r\n", getpid(), pid, status);
        exit(2); //echo $?
    }
}

static void demo_wait_sigchild()
{
    pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed /r/n");
        return ;
    } else if(pid == 0) {
        usleep(1000000);
        exit(0);
    }
    signal(SIGCHLD, sigchild_handler);
    pause();
}

int main()
{
//    demo_wait_sigchild();
//    demo_wait_wifstatus();
//    demo_wait_secfork();
    demo_test_competition();
    exit(0);
}
