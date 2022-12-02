#if 0
    1 问题发生（demo_sigblock_pause()）：
        sigprocmask(SIG_BLOCK, set)-->sigprocmask(SIG_UNBLOCK, set)-->pause()等待被阻塞的信号
        如果在阻塞期间收到信号（mask bit = 0, pending bit = 1），信号传递被阻塞，解除阻塞时(
        mask bit = 1，pending bit = 1)出内核会发生信号，调用信号处理函数，pause将使进程永远挂起

    2. 解决办法：将解除阻塞和pause操作作为原子操作
        int sigsuspend(const sigset_t *mask);
        恢复信号屏蔽字，并使进程休眠（原子操作）
        @mask : 将要设置的信号屏蔽字
        demo_sigsuspend()
        
    3 另一个应用：等待一个全局变量被设置(demo_wait_globalset())(父子进程间的同步)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void print_sigmask(const char *str)
{
    sigset_t mask;
    printf("sigmask ");
    printf("%s", str);
    if(sigprocmask(SIG_SETMASK, NULL, &mask) < 0) {
        fprintf(stderr, "get sigprocmask failed \r\n");
        return ;
    }
    printf(" : ");
    if(sigismember(&mask, SIGINT)) {
        printf("SIGINT ");
    }
    if(sigismember(&mask, SIGUSR1) ) {
        printf("SIGUSR1 ");
    }
    if(sigismember(&mask, SIGALRM)) {
        printf("SIGALRM ");
    }
    putchar('\n');
}
static void sigalrm_handler(int signum)
{
    printf("SIGALRM received \r\n");
    print_sigmask("sigalrm handler");
}

/*
 * @error: proccess will pause forever
 * */
static int demo_sigblock_pause()
{
    sigset_t set, oldset;
    if(signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
        fprintf(stderr, "failed to register SIGALRM \r\n");
        return -1;
    }
    if(sigemptyset(&set) < 0) {
        fprintf(stderr, "failed to sigemptyset \r\n");
        return -1;
    }
    if(sigaddset(&set, SIGALRM) < 0) {
        fprintf(stderr, "failed to add SIGALRM to sigset");
        return -1;
    }
    if(sigprocmask(SIG_BLOCK, &set, &oldset) < 0) {
        fprintf(stderr, "failed to sigprocmask! \r\n");
        return -1;
    }
    alarm(1);
    /* protected task start */
    printf("sleep begin \r\n");
    usleep(2000000);
    printf("sleep end \r\n");
    /* protected task end */
    if(sigprocmask(SIG_SETMASK, &oldset, NULL) < 0) {
        fprintf(stderr, "failed to sigprocmask! \r\n");
        return -1;
    }
    pause();
}

static int demo_sigsuspend()
{
    sigset_t set, oldset, waitmask;
    if(signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
        fprintf(stderr, "failed to register SIGALRM \r\n");
        return -1;
    }
    if(sigemptyset(&set) < 0) {
        fprintf(stderr, "failed to sigemptyset");
        return -1;
    }
    if(sigaddset(&set, SIGALRM) < 0) {
        fprintf(stderr, "failed to sigaddset \r\n");
        return -1;
    }
    if(sigemptyset(&waitmask) < 0) {
        fprintf(stderr, "failed to sigemptyset");
        return -1;
    }
    if(sigaddset(&waitmask, SIGINT) < 0) {
        fprintf(stderr, "failed to sigaddset \r\n");
        return -1;
    }
    print_sigmask("sigpromask before");
    if(sigprocmask(SIG_BLOCK, &set, &oldset) < 0) {
        fprintf(stderr, "failed to sigprocmask \r\n");
        return -1;
    }
    print_sigmask("alarm before");
    alarm(1);
    sleep(5);
    print_sigmask("suspend before");
    sigsuspend(&waitmask);
    print_sigmask("suspend end");
    if(sigprocmask(SIG_SETMASK, &oldset, NULL) <0) {      /* recoverd promask */
        fprintf(stderr, "failed to recoverd sigmask \r\n");
    }
    print_sigmask("recoverd end");
    return 0;
}

static volatile int startFlag;

static void sigtaskstart_handler(int signum)
{
    if(signum = SIGUSR1) {
        startFlag = 1;
    }
}

static int demo_wait_globalset(int signum)
{
    sigset_t newmask, oldmask, zeromask;
    sigemptyset(&newmask);
    sigemptyset(&zeromask);
    sigaddset(&newmask, signum);
    if(signal(signum, sigtaskstart_handler) == SIG_ERR) {
        fprintf(stderr, "failed to register signal \r\n");
        return -1;
    }
    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        fprintf(stderr, "failed to sigprocmask \r\n");
        return -1;
    }
    startFlag = 0;
    while(!startFlag) sigsuspend(&zeromask);
    startFlag = 0;
    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        fprintf(stderr, "failed to recoverd sigmask \r\n");
        return -1;
    }
    return 0;
}

int main()
{
//    int ret = demo_sigblock_pause();
//    int ret = demo_sigsuspend();
    
    /* task wait */
    int ret;
    pid_t pid;
    const int signum = SIGUSR1;
    if((pid = fork()) < 0) {
        fprintf(stderr, "failed to fork child process \r\n");
        exit(-1);
    } else if(pid == 0) {
        printf("child wait signal to start task \r\n");
        ret = demo_wait_globalset(signum);
        if(ret < 0)
            fprintf(stderr, "error for wait for task \r\n");
        else {
            printf("this is child task start! \r\n");
            usleep(10000000);
            printf("child task end! \r\n");
            kill(getppid(), signum);
        }
    } else {
        int status;
        usleep(5000000);
        printf("send SIGUSR1 to child \r\n");
        kill(pid, signum);
        
        /* wait child task end */
        printf("wait child end \r\n");
        /* 小问题：父进程这里在子进程发送信号后调用此函数会发生信号丢失 */
        ret = demo_wait_globalset(signum);
        if(ret < 0)
            fprintf(stderr, "error for wait for task \r\n");
        else {
            printf("parent proc task ! \r\n");
        }
        waitpid(pid, &status, 0);
        printf("child end status is %d \r\n", status);
        ret = status;
    }
    exit(ret);
}
