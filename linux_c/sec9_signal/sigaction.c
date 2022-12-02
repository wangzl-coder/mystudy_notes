#if 0
    1. 
        int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
        修改或检查与指定信号相关的处理动作
        
        struct sigaction {
            void (*sa_handler)(int);
            void (*sa_sigaction)(int, siginfo_t *, void *);
            sigset_t mask;
            int sa_flags;
            void (*sa_restorer)(void);
        };
        sa_handler : 信号处理程序
        mask: 调用信号处理程序前，将mask集合中的信号集加入到进程的信号屏蔽字中（阻塞这些信号）
        sa_flags:   SA_INTERRUPT(中断系统调用不自动重启)
                    SA_NOCLDSTOP:
                    ...
                    SA_SIGINFO:使用详细的信号处理程序
        sa_sigaction: 详细的信号处理程序，与sa_handler为共用体

#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

static void sig_handler(int signum)
{
    int i;
    if(signum == SIGUSR1) {
        printf("SIGUSR1 start \r\n"); 
        for(i = 0; i < 1000000000; i++);
        printf("SIGUSR1 end \r\n"); 
    } else if(signum == SIGUSR2) {
        printf("SIGUSR2 received \r\n");
    }
}

static int demo_sig_norestar()
{
    pid_t pid;
    if((pid = fork()) < 0) {
        fprintf(stderr, "failed to fork \r\n");
        return -1;
    } else if(pid == 0) {
        char buf[1024];
        void (*oldhandler)(int);
        struct sigaction act, oldact;
        int len;
        int ret = 0;
#if 0
        if((oldhandler = signal(SIGUSR1, sig_handler) ) == SIG_ERR) {
            fprintf(stderr, "register SIGUSR1 failed \r\n");
            return -1;
        }
#else
        sigemptyset(&act.sa_mask);
        act.sa_handler = sig_handler;
        act.sa_flags = SA_INTERRUPT;
        if(sigaction(SIGUSR1, &act, &oldact) < 0) {
            fprintf(stderr, "failed to sigaction \r\n");
            return -1;
        }
#endif
        if((len = read(STDIN_FILENO, buf, sizeof(buf) - 1)) < 0) {
            if(errno == EINTR)
                printf("read : interrupt by a signal \r\n");
            fprintf(stderr, "read from stdint failed \r\n");
            ret = -1;
            goto reset_hander;
        }
        if(write(STDOUT_FILENO, buf, len) != len) {
            fprintf(stderr, "failed to write to stdout \r\n");
            ret = -1;
        }

    reset_hander:
#if 0
        if(signal(SIGUSR1, oldhandler) == SIG_ERR)
            fprintf(stderr, "Warning: failed to reset handler for SIGUSR1 \r\n");
#else
        if(sigaction(SIGUSR1, &oldact, NULL) < 0) {
            fprintf(stderr, "Warning : failed to reset sigaction for SIGUSR1 \r\n");
        }
#endif
        return ret;
    } else {
        usleep(10000);
        kill(pid, SIGUSR1);
        waitpid(pid, NULL, 0);
        return 0;
    }
}

static int sigaction_demo01(int handlesig, int masksig)
{
    sigset_t set;
    struct sigaction act, oldact;
    if((sigemptyset(&set)) < 0) {
        perror("sigemptyset failed ");
        return -1;
    }
    if(sigaddset(&set, masksig) < 0) {
        perror("sigaddset failed ");
        return -1;
    }

    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    act.sa_mask = set;
    if(sigaction(handlesig, &act, &oldact) < 0) {
        fprintf(stderr, "sigaction failed \r\n");
        return -1;
    }
    
    pause();
    printf("pause eixt \r\n");
    return 0;
}

/*
 * 注：如果masksig为SIGUSR1，阻塞结束后信号处理函数将会处理一次
 **/
int main()
{
    int ret;
#if 0
    pid_t pid;
    int handlesig = SIGUSR1, masksig = SIGUSR2;
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        exit(-1);
    } else if(pid == 0) {
        if(signal(handlesig, sig_handler) == SIG_ERR) {
            fprintf(stderr, "failed to regiser SIGUSR2");
            exit(-1);
        }
        if(signal(masksig, sig_handler) == SIG_ERR) {
            fprintf(stderr, "failed to regiser SIGUSR2");
            exit(-1);
        }
        ret = sigaction_demo01(handlesig, masksig);
    } else {
        int status;
        usleep(100);
        kill(pid, handlesig);
        kill(pid, masksig);
        waitpid(pid, &status, 0);
        printf("wait %d status = %d \r\n", pid, status);
        ret = 0;
    }
#endif
    ret = demo_sig_norestar();
    exit(ret);
}
