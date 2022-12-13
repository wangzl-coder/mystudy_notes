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
