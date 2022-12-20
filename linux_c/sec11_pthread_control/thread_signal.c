#if 0
    1 线程和信号：
        每个线程都有自己的信号屏蔽字，但是信号的处理实在进程中所有的线程共享的。单个线程
        可以阻止某些信号
        进程中的信号是传递到单个线程的

    2 
        int pthread_sigmask(int how, const sigset_t *restrict set, sigset_t *restrict oldset);
        修改线程的信号屏蔽字，注意和sigprocmask函数区别
        @how: SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK

    3 
        int sigwait(const sigset_t *restrict set, int *restrict signop);
        线程等待一个或多个信号的出现：
        @set: 指定线程等待的信号集
        sigwait 会原子的取消信号集的阻塞状态，直到有新的信号被递送，在返回之前，sigwait将恢复信号屏蔽字。
        为了避免错误行为发生，线程在调用sigwait之前，必须阻塞那些它正在等待的信号
        
        好处：简化信号处理，允许异步产生的信号用同步的方式处理。信号处理程序运行在正常的线程上下文，而不是
        中断线程正常执行的传统信号处理程序
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t quit_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t quit_cond = PTHREAD_COND_INITIALIZER;
static bool quit_flag;

void *thread_func(void *arg)
{
    int signum;
    sigset_t *mask = arg;
    int err;

    for(;;) {
        err = sigwait(mask, &signum);
        if(err) {
            fprintf(stderr, "warnning : sigwait error : %s \r\n", strerror(err));
            continue;
        }
        printf("signal handler thread: ");
        switch(signum) {
            case SIGQUIT:
                printf("%s received \r\n", strsignal(signum));
                pthread_mutex_lock(&quit_lock);
                quit_flag = true;
                pthread_mutex_unlock(&quit_lock);
                pthread_cond_signal(&quit_cond);
                return (void *)0;
                break;
            case SIGINT:
                printf("%s received \r\n", strsignal(signum));
                break;
            default:
                fprintf(stderr, "warning: unexpected result \r\n");
                break;
        }
    }
    pthread_exit((void *)0);
}

int main()
{
    int ret;
    long *retval;
    pthread_t tid;
    sigset_t mask, oldmask;
    ret = sigemptyset(&mask);
    if(ret < 0) {
        fprintf(stderr, "failed to sigemptyset mask \r\n");
        exit(ret);
    }
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    ret = pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
    if(ret) {
        fprintf(stderr, "failed to pthread_sigmask : %s \r\n", strerror(ret));
        exit(ret);
    }
    ret = pthread_create(&tid, NULL, thread_func, &mask);
    if(ret) {
        fprintf(stderr, "failed to create pthread : %s \r\n", strerror(ret));
        goto revert_mask;
    }
    pthread_mutex_lock(&quit_lock);
    while(!quit_flag) {
        pthread_cond_wait(&quit_cond, &quit_lock);
    }
    pthread_mutex_unlock(&quit_lock);
    pthread_join(tid, (void **)&retval);
    printf("exit : status is %ld \r\n",(long) retval);

revert_mask:
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    exit(ret);
}
