#if 0
    1 超时锁：
        int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex, 
                                    const struct timespec *restrict tsptr);
    
        @mutex: 要获取的锁
        @tsptr : 指定超时时间，相对时间，表示在当前时间之前阻塞等待，
        @ return : 成功获取锁，返回0。出错返回错误编号，返回ETIMEDOUT时表示阻塞超时
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


int main(void)
{
    int err;
    struct timespec tp;
    struct tm *tmp;
    pthread_mutex_t mutex;
    char buf[20];
    
    if((err = pthread_mutex_init(&mutex, NULL))) {
        fprintf(stderr, "failed to init mutex : %s \r\n", strerror(err));
        exit(-1);
    }
    pthread_mutex_lock(&mutex);
    clock_gettime(CLOCK_REALTIME, &tp);
    tmp = localtime(&tp.tv_sec);
    strftime(buf, sizeof(buf), "%r", tmp);
    printf("before time is %s \r\n", buf);
    tp.tv_sec += 10;
    err = pthread_mutex_timedlock(&mutex, &tp);
    tmp = localtime(&tp.tv_sec);
    strftime(buf, sizeof(buf), "%r", tmp);
    printf("after time is %s \r\n", buf);
    if(!err) {
        printf("mutex lock again \r\n");
    } else {
        printf("mutex lock failed : %s \r\n", strerror(err));
    }
    exit(0);
}
