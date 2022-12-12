#if 0
    1 线程同步：当多线程对变量的修改在处理器对存储器的访问周期（memory->register->change->memory）间时（周期交叉），会出现数据不同步的情况
    
    2 解决方法，将读写作为原子操作

    3 互斥量（mutex）：保护数据的互斥接口，同一时间只有一个线程访问数据。互斥量本质是一把锁，访问前加锁，加锁后其他访问资源的线程获取锁时将被阻塞。
        访问完成释放锁，其他线程成为可运行状态，进行抢占锁
    
    4 互斥锁：pthread_mutex_t , 使用前初始化，静态初始化PTHREAD_MUTEX_INITIALIZER,动态初始化pthread_mutex_init,动态初始化使用完后需要使用
        pthread_mutex_destroy进行释放

    5 
        int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);
        int pthread_mutex_destroy(pthread_mutex_t *mutex);
        @return : 0 for success,or error number for error

    6
        int pthread_mutex_lock(pthread_mutex_t *mutex);
        对互斥量加锁，如果已经被上锁，调用线程将被阻塞
        int pthread_mutex_trylock(pthread_mutex_t *mutex);
        加锁，如果已经被上锁，不会阻塞，返回EBUSY
        int pthread_mutex_unlock(pthread_mutex_t *mutex);
        释放锁

        @return : 0 for success, or error num for error
        demo : 对象的引用保护
    
    7 死锁：显式：对同一个互斥量加锁两次，会产生死锁现象。
            隐式：多个线程访问多个互斥量，产生交叉加锁时会出现死锁，解决办法：（1）顺序访问 （2）适当时让一个线程放弃锁，过段时间重试(demo_hash_table.c)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

        
struct foo{
    int count;
    int id;
    pthread_mutex_t mutex;
};

foo *foo_alloc(int id)
{
    struct foo *fp;
    if((fp == malloc(sizeof(struct foo))) == NULL) {
        fprintf(stderr, "Warnning :malloc for foo failed \r\n");
        return NULL;
    }
    fp->id = id;
    fp->count = 1;
    if(pthread_mutex_init(&fp->mutex, NULL) != 0) {
        fprintf(stderr, "Warnning : mutex init failed \r\n");
        free(fp);
        return NULL;
    }
    return fp;
}


void foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&fp->mutex);
    fp->count ++;
    phtead_mutex_unlock(&fp->mutex);
}

void foo_rele(struct foo *fp)
{
    pthread_mutex_lock(&fp->mutex);
    if(--fp->count == 0) {
        pthread_mutex_unlock(&fp->mutex);
        pthread_mutex_destroy(&fp->mutex);
        free(fp);
    } else
        pthread_mutex_unlock(&fp->mutex);
}
