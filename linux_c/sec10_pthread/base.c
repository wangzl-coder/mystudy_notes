#if 0
    1 线程概念：
        a. 进程内内存空间共享
        b. 同步编程模式，比异步简单很多
        c. 可以访问相同的存储地址空间和文件描述符
        d. 问题的分解提高程序吞吐量马，交叉执行（互相不依赖）
        e. 交互程序可优化响应时间
        f. 每个线程都包含表示执行环境所必须的信息，包括线程ID，一组寄存器值，栈，调度优先级和策略
            信号屏蔽字，error变量以及线程私有数据，一个进程的所有信息对该进程所有的线程都是共享的
            包括可执行程序的代码，程序的全局内存和堆内存，栈以及文件描述符
        g. POSIX.线程标准
    2. 线程标识
        a. 线程ID：只有在它所属的进程上下文中才有意义
        b. pthread_t ,不能将其作为整数处理（可移植性）（linux使用无符号长整型）
        c. int pthread_equal(pthread_t tid1, pthread_t tid2);相等返回非0，反之0
        d. pthread_t pthread_self(void);主要用来在多线程环境下指定作业控制

    3. 线程创建：
        int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
        0 for success, or return errno
        
        注意：创建时不能保证哪个线程先运行，新创建的线程可以访问进程的地址空间，并且继承调用线程的浮点环境和信号屏蔽字
            但是该线程的挂起信号集会被清楚
    
    4. 线程的终止：
        a. 任意线程调用了exit, _Exit, 或者_exit，整个进程就会终止，如果默认动作时终止进程，那么发送到线程的信号就会终止整个进程
        b. 单个线程的退出方式：
            （1）从启动例程中返回，返回值是线程的退出码
            （2）被同一进程中的其他线程取消
            （3）线程调用pthread_exit
        c. void pthread_exit(void *retval); @retval : 退出码

        d. int pthread_join(pthread_t thread, void **retval) (0 for success, or return errno)
            (1) 调用线程一直阻塞，知道指定进程调用pthread_exit, 从启动例程中返回或者被取消，如果线程简单的从他的启动例程返回，则
            retval包含返回码，如果线程被取消，又retval指定的内存单元被设置为PTHREAD_CANCELED
            (2) 线程分离，pthread_join调用失败，返回EINVAL

#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


static void *thread_func1(void *arg)
{
    printf("thread 1 return \r\n");
    return (void *)1;
}

static void *thread_func2(void *arg)
{
    printf("thread 2 return \r\n");
    pthread_exit((void *)2);
}

static int demo_pthread_join()
{
    int err;
    pthread_t tid1, tid2;
    void *retval;
    err = pthread_create(&tid1, NULL, thread_func1, NULL);
    if(err) {
        fprintf(stderr, "failed to create thread tid1 : %s \r\n", strerror(err));
        return err;
    }
    err = pthread_create(&tid2, NULL, thread_func2, NULL);
    if(err) {
        fprintf(stderr, "failed to create thread tid2 : %s \r\n", strerror(err));
        return err;
    }

    err = pthread_join(tid1, &retval);
    if(err) {
        fprintf(stderr, "failed of join tid1 : %s \r\n", strerror(err));
        return err;
    }
    printf("retval of tid1 is %p \r\n", retval);
    err = pthread_join(tid2, &retval);
    if(err) {
        fprintf(stderr, "failed of join tid2 : %s \r\n", strerror(err));
        return err;
    }
    printf("retval of tid2 is %p \r\n", retval);
    return err;
}


/* 测试打印线程ID，仅用于测试 */
static void printtids(const char *s)
{
    pid_t pid;
    pthread_t tid;


    pid = getpid();
    tid = pthread_self();

    printf("%s pid = %lu, tid = %lu (0x%lx) \r\n", s, (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

static void *thread_func(void *arg)
{
    printtids("child thread : ");
    
    return (void *)0;
}

int main()
{
    int err;
    pthread_t tid;

    if(err = pthread_create(&tid, NULL, thread_func, NULL)) {
        fprintf(stderr, "create pthread failed : %s \r\n", strerror(err));
        exit(-1);
    }
    usleep(1000000);
    printtids("parent thread : ");
    err = demo_pthread_join();
    exit(0);
}
