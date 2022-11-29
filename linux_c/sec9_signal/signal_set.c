#if 0
    1 信号集(sigset_t)
        a. 
            int sigemptyset(sigset_t *set); //初始化清空信号集set
            int sigfillset(sigset_t *set);  //初始化信号集使其包含所有信号
            int sigaddset(sigset_t *set, int signum);   //向已初始化的信号集set添加信号signum
            int sigdelset(sigset_t *set, int signum);   //从已初始化的信号集set删除信号signum
                @return :0 for success, -1 for error    
            int sigismember(const sigset_t *set, int signum);   //测试信号signum是否存在于set中
        
        b. 
            int sigpromask(int how, const sigset_t *set, sigset_t *oldset);
            检测更改进程信号屏蔽字（mask）,
            @how : SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
        
        c. 
            int sigpending(sigset_t *set);
#endif

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>

static volatile int i;
static void siguser1_handler(int signum)
{
    char buf[90];
    sprintf(buf, "siguser1 received, i = %d\r\n", i);
    write(STDOUT_FILENO, buf, strlen(buf));
}

static int sigprocmask_demo()
{
    pid_t pid;
    sigset_t set, oldset;
    if(signal(SIGUSR1, siguser1_handler) == SIG_ERR) {
        fprintf(stderr, "SIGUSR1 failed \r\n");
        return -1;
    }
    if((pid = fork()) < 0)
        fprintf(stderr, "warning : fork failed \r\n");
    else if(pid == 0) {
        if(sigemptyset(&set) < 0)
            perror("sigemptyset error");
        sigaddset(&set, SIGUSR1);
        /* set block for SIG_BLOCK */
        if(sigprocmask(SIG_BLOCK, &set, &oldset) < 0)
            perror("sigpromask failed ");
        
        /* task that we don't want be interrupted by signal */
        printf("child pro task start \r\n");
        for(i = 0; i < 100000000; i++);

        /* task end, recovered it (set unblock or recovered to oldset) */
        //if(sigprocmask(SIG_UNBLOCK, &set, NULL) <0)
        if(sigprocmask(SIG_SETMASK, &oldset, NULL) < 0)
            perror("sigpromask failed ");
        exit(0);
    }else {
        /* schedule child (no't a good way)*/
        usleep(100);
        printf("signal SIGUSR1 to child \r\n");
        kill(pid, SIGUSR1);
        waitpid(pid, NULL, 0);
        return 0;
    }
}

int main()
{
    int ret = sigprocmask_demo();
    exit(ret);
}
