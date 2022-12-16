#if 0
    线程属性：pthread_attr_t 
        int pthread_attr_init(pthread_attr_t *attr);
        int pthread_attr_destory(pthread_attr_t *attr);

    1 线程分离：对现有线程的终止状态不关心，可以使其分离，操作系统在线程退出时收回其占用的资源
        创建后设置：int pthread_detach(pthread_t thread);
        创建时设置：int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
        获取：int pthread_attr_getdetachstate(pthread_attr_t *attr, int *detachstate);
        @return :0 for sunccess ; or error number
        @detachstate : PTHREAD_CREATE_DETACHED, PTHREAD_CREATE_JOINABLE
    
    2 线程栈:
        int pthread_attr_getstatck(const pthread_attr_t *restrict attr, 
                                    void **restrict stackaddr,
                                    size_t *restrict stacksize);
        int pthread_attr_setstack(pthread_attr_t *attr,
                                void *stackaddr, size_t stacksize);
        
        @stackaddr : 线程栈的最低地址(并非起始地址);
        @stacksize : 线程栈大小
        
        int pthread_attr_getstacksize(pthread_attr_t *restrict attr,
                                        size_t *restrict stacksize);
        int pthread_attr_setstacksize(pthread_attr_t *restrict attr,
                                        size_t stacksize);
        
        线程栈警戒缓冲区大小
        int pthread_attr_getguardsize(pthread_attr_t *restrict attr, size_t *restrict guardsize);
        int pthread_attr_setguardsize(pthread_attr_t *restrict attr, size_t guardsize);
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

static void *thread_func(void *arg)
{
    pthread_attr_t attr;
    int detachstate;
    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getdetachstate(&attr, &detachstate);
    if(detachstate == PTHREAD_CREATE_DETACHED) {
        printf("thread detachstate is detached \r\n");
    } else if(detachstate == PTHREAD_CREATE_JOINABLE) {
        printf("thread detachstate is joinable \r\n");
    }
    return (void *)0;
}

int main() 
{   
    int err;
    pthread_t tid;
    pthread_attr_t attr;
    err = pthread_attr_init(&attr);
    if(err) {
        fprintf(stderr, "failed to init thread : %s \r\n",strerror(err));
        exit(-1);
    }
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(err) {
        fprintf(stderr, "failed to init thread : %s \r\n", strerror(err));
        goto attr_destroy;
    }
    err = pthread_create(&tid, &attr, thread_func, NULL);
    if(err) {
        fprintf(stderr, "failed to create thread : %s \r\n", strerror(err));
        goto attr_destroy;
    }
    err = pthread_join(tid, NULL);
    if(err) {
        fprintf(stderr, "failed to wait thread : %s \r\n", strerror(err));
    }
attr_destroy:
    pthread_attr_destroy(&attr);
    while(1);
    exit(err);
}
