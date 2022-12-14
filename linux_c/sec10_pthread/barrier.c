#if 0
    1 屏障：协调多个线程并行的同步机制。屏障允许所有线程等待，直到所有的合作线程都到达某一点。然后从该点继续执行。pthread_join
        就是一种屏障，允许一个线程等待，直到另一个线程退出。

    2 初始化和反初始化
        int pthread_barrier_init(pthread_barrier_t *restrict barrier,
                                const pthread_barrierattr_t *restrict attr,
                                unsigned int count);
        int pthread_barrier_destroy(pthread_barrier_t *restrict barrier);

    3 屏障等待：
        int pthread_barrier_wait(pthread_barrier_t barrier);
        @return : if success return 0 or PTHREAD_BARRIER_SERIAL_THREAD. if error return error number
        
        屏障计数不满足时，调用线程进入休眠状态，直到屏障计数满足，所有线程被唤醒
#endif

/* demo：多个线程对数组并行分段排序 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>

#define NUMNUM 8000000L     /* number of num that need sort */
#define NTHREAD 8           /* number of thread to sort */
#define NUM_PER_THREAD  (NUMNUM/NTHREAD)    /* number of num for every thread to sort */

static long int num[NUMNUM];
static long int sumnum[NUMNUM];
pthread_barrier_t barrier;

static int long_int_compare(const void *arg1, const void *arg2)
{
    long int num1 = *(long int *)arg1;
    long int num2 = *(long int *)arg2;
    return (num1 == num2)?0:((num1 > num2)?1:-1);
}

static void *thread_func(void *arg)
{
    long index = (long)arg;
    int i;
    qsort(&num[index], NUM_PER_THREAD, sizeof(long int), long_int_compare);
    pthread_barrier_wait(&barrier);
    
    return (void *)0;
}

static void num_merge()
{
    int i;
    long sumindex;
    long per_startindex[NTHREAD];
    long int minnum;
    int min_sec;

    for(i = 0; i < NTHREAD; i++) {
        per_startindex[i] = i*NUM_PER_THREAD;
    }
    for(sumindex = 0; sumindex < NUMNUM; sumindex ++) {
        minnum = LONG_MAX;
        for(i = 0; i < NTHREAD; i++) {
            if(per_startindex[i] < ((i + 1)*NUM_PER_THREAD) && num[per_startindex[i]] < minnum) {      /* 被取完的段应该跳过 */
                minnum = num[per_startindex[i]];
                min_sec = i;
            }
        }
        sumnum[sumindex] = minnum;
        per_startindex[min_sec] ++;
    }
}

int main(void)
{
    int i;
    int err;
    pthread_t tid[NTHREAD];

    for(i = 0; i < NUMNUM; i++) {
        num[i] = random()%100;
    }
    if((err = pthread_barrier_init(&barrier, NULL, NTHREAD + 1))) {   /* contain main thread.so + 1 */
        fprintf(stderr, "failed to init barrier : %s \r\n", strerror(err));
        exit(-1);
    }
    for(i = 0; i < NTHREAD; i++) {
        err = pthread_create(&tid[i], NULL, thread_func, (void *)(i * NUM_PER_THREAD));
    }
    pthread_barrier_wait(&barrier);
    printf("thread task complete ! \r\n");
    num_merge();
    for(i = 0; i < NTHREAD; i++) {
        long *ret;
        pthread_join(tid[i], (void **)&ret);
        printf("ret val is %ld \r\n",(long int)ret);
    }
    exit(0);
}
