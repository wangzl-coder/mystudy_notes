#if 0
    1. int pthread_cancel(pthread_t thread);
        请求（建议）取消同一进程中的其他线程
    
    2. 线程清理处理程序
        void phtread_cleanup_push(void (*routin) (void *), void *arg);
        注册线程清理处理程序（栈中，先注册的后执行）
        调用时机：
            a. 调用pthread_exit(注意从处理程序return终止不会调用);
            b. 响应取消请求
            c. 用非零execute参数调用pthread_cleanup_pop
        void pthread_cleanup_pop(int execute);
        如果execute设置为0，清理函数将不被调用。发生上述三种情况时，将删除上次注册的清理程序
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void cleanup(void *arg)
{
    printf("cleanup : %s \r\n", (char *)arg);
}

/* pthread_cleanup_push 和pthread_cleanup_pop为宏定义，为了使得编译通过，需要配合使用 */
static void *thread_func1(void *arg)
{
    printf("thread 1 start! \r\n");
    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");
    printf("thread 1 push complete \r\n");
    if(arg)
        return (void *)1;
    /* just for comppile  */
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void *)1;
}

static void *thread_func2(void *arg)
{
    printf("thread 2 start! \r\n");
    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");
    printf("thread 2 push complete \r\n");
    if(arg)
        pthread_exit((void *)2);
    /* just for comppile  */
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    pthread_exit((void *)2);
}

int main()
{
    int ret;
    pthread_t tid1, tid2;
    int *status;

    ret = pthread_create(&tid1, NULL, thread_func1, (void *)1); /* param (void *)1 avoid to be pop in thread handler */
    if(ret) {
        fprintf(stderr, "fail to create thread 1 : %s\r\n", strerror(ret));
        exit(ret);
    }
    ret = pthread_create(&tid2, NULL, thread_func2, (void *)1);
    if(ret) {
        fprintf(stderr, "fail to create thread 2 : %s \r\n", strerror(ret));
        goto thread2_failed;
    }
    pthread_join(tid2, (void **) &status);
    printf("thread 2 exit code %ld \r\n", (long) status); /* for 64 bit system,so be long */
thread2_failed:
    pthread_join(tid1, (void **) &status);
    printf("thread 1 exit code %ld \r\n", (long) status);
    exit(ret);
}
