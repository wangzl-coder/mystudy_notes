#if 0
    1 子进程通过继承父进程整个地址空间的副本，还从父进程那儿继承了每个互斥量，读写锁和条件变量的状态
    ，如果父进程包含一个以上的线程，子进程fork返回后，如果不是马上调用exec的话，就需要清理锁状态
    2 在子进程内部，只存在一个线程，它是由父进程调用fork的线程的副本构成的。如果父进程中的线程占有锁
    子进程将同样占有这些锁，问题是子进程并不包含占有锁的线程的副本，所以子进程没有办法知道它占有了
    哪些锁，需要释放哪些锁。

    3 清除锁状态：
        int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
        @prepare: 父进程在fork创建子进程前调用，任务：获取父进程定义的所有锁
        @parent: fork创建子进程之后，返回之前在父进程上下文调用。
        @child: fork返回之前在子进程上下文中调用，
        parent fork 和child fork 必须释放prepare fork中获取的所有锁

        多次注册：parent 和 child fork以注册顺序调用；prepare fork调用顺序与注册时顺序相反
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;


void *thread_func1(void *arg)
{
    pthread_mutex_lock(&lock1);
    printf("thread_func1 catch lock1 \r\n");
    usleep(2000000);
    pthread_mutex_lock(&lock2);
    printf("thread_func1 catch lock2 \r\n");
    usleep(2000000);
    pthread_mutex_unlock(&lock1);
    printf("thread_func1 release lock1 \r\n");
    usleep(2000000);
    pthread_mutex_unlock(&lock2);
    printf("thread_func1 release lock2 \r\n");
    return (void *)0;
}

void *thread_func2(void *arg)
{
    pid_t pid;
    printf("thread_func2 prepare to fork \r\n");
    pid = fork();
    if(pid < 0) {
        fprintf(stderr, "failed to fork \r\n");
        return (void *)-1;
    } else if(pid == 0) {
        printf("child fork return \r\n");
        exit(0);
    } else {
        int status;
        printf("parent fork return \r\n");
        waitpid(pid, &status, 0);
        printf("child exit status is %d \r\n", status);
    }
    return (void *)0;
}

static void prepare1()
{
    printf("prepaer1 \r\n");
    pthread_mutex_lock(&lock1);
    printf("parent catch lock1 \r\n");
}

static void parent1()
{
    pthread_mutex_unlock(&lock1);
    printf("parent release lock1 \r\n");
}

static void child1()
{
    pthread_mutex_unlock(&lock1);
    printf("child release lock1 \r\n");
}

static void prepare2()
{
    printf("prepaer2 \r\n");
    pthread_mutex_lock(&lock2);
    printf("parent catch lock2 \r\n");
}

static void parent2()
{
    pthread_mutex_unlock(&lock2);
    printf("parent release lock2 \r\n");
}

static void child2()
{
    pthread_mutex_unlock(&lock2);
    printf("child release lock2 \r\n");
}

int main()
{
    pthread_t tid1, tid2;
    long *retval;

    pthread_atfork(prepare2, parent2, child2);
    pthread_atfork(prepare1, parent1, child1);
    pthread_create(&tid1, NULL, thread_func1, NULL);
    pthread_create(&tid2, NULL, thread_func2, NULL);
    
    pthread_join(tid1,(void **)&retval);
    printf("thread 1 return %ld \r\n", (long)retval);
    pthread_join(tid2, (void **)&retval);
    printf("thread 2 return %ld \r\n", (long)retval);
    exit(0);
}

