#include <stdio.h>
#include <stdlib.h>


#define NHASH 29
#define hash(id) (((unsigned long)id)%NHASH)

struct foo *ftable[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo{
    int id;
    int count;
    pthread_mutex_t f_mutex;
    struct foo *f_next;

    /* more stuff
     ....   */
};

struct foo *foo_alloc(int id)
{
    struct foo *fp;
    if((fp = malloc(sizeof(struct foo))) == NULL) {
        return NULL;
    }
    fp->count = 1;
    fp->id = id;
    if(pthread_mutex_init(&fp->f_mutex, NULL) != 0) {
        free(fp);
        return NULL;
    }
    pthread_mutex_lock(&hashlock);
    fp->f_next = ftable[hash(id)];
    ftable[hash(id)] = fp;
    pthread_mutex_lock(&fp->f_mutex);
    pthread_mutex_unlock(&hashlock);
    /* continue initialization
                             ... */
    pthread_mutex_unlock(&hashlock);
}

void foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_mutex);
    fp->count ++;
    pthread_mutex_unlock(&fp->f_mutex);
}

struct foo *foo_find(int id)
{   
    struct foo *fp;

    pthread_mutex_lock(&hashlock);
    for(fp = ftable[hash(id)]; fp != NULL; fp = fp->next) {
        if(fp->id == id) {
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}


struct void foo_rele(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_mutex);
    struct foo *f_curr;
    if(fp->count == 1) {    /* last refrence */
        pthread_mutex_unlock(&fp->f_mutex); /* 需要修改散列表，重新lock保持顺序，避免死锁 */
        pthread_mutex_lock(&hashlock);
        pthread_muex_lock(&fp->f_mutex);
        if(fp->count != 1) {      /* lock f_mutex时可能阻塞，需要重新检测 */
            /* refrence */
            fp->count --;
            pthread_mutex_unlock(&fp->f_mutex);
            pthread_mutex_unlock(&hashlock);
            return ;
        }
        /* last refrence , free it */       /* 从散列表中删除 */
        f_curr = ftable[hash(id)];
        if(f_curr = fp)
            ftable[hash(id)] = fp->next;
        else {
            while(f_curr->f_next != fp) {
                f_curr = f_curr->f_next;
            }
            f_curr->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&fp->f_mutex);
        pthread_mutex_unlock(&hashlock);
        free(fp);
    } else {
        fp->count --;
        pthread_mutex_unlock(&fp->f_mutex);
    }
}

/* 优化，可用散列表锁来保护数据引用结构，简化代码 */

struct foo *foo_alloc(int id)
{
    struct foo *fp;
    if((fp = malloc(sizeof(struct foo))) == NULL) {
        fprintf(stderr, "Error: failed to malloc for struct foo! \r\n");
        return NULL;
    }
    fp->count = 1;
    fp->id = id;
    if(pthread_mutex_init(&fp->f_mutex, NULL)){
        fprintf(stderr, "Error: failed to init fp->f_mutex \r\n");
        free(fp);
        return NULL;
    }
    pthread_mutex_lock(&hashlock);
    fp->f_next = ftable[hash(id)];
    ftable[hash(id)] = fp;
    pthread_mutex_lock(&fp->f_mutex);
    pthread_mutex_unlock(&hashlock);
    /* continue initialization */
    pthread_mutex_unlock(&fp->f_mutex);
    return fp;
}

void foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&hashlock);
    fp->count ++;
    pthread_mutex_unlock(&hashlock);
}

struct foo *foo_find(int id)
{
    struct foo *fp;

    pthread_mutex_lock(&hashlock);
    for(fp = ftable[hash(id)]; fp != NULL; fp = fp->next) {
        if(fp->id == id)
            break;
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void foo_rele(struct foo *fp)
{
    struct foo *tfp;
    int index;

    pthread_mutex_lock(&hashlock);
    if(--fp->count == 0) {          /* last refrence */
        index = hash(id);
        tfp = ftable[index];
        if(tfp == fp) {
            ftable[index] = fp->next;
        } else {
            while(tfp->next != fp) {
                tfp = tfp->next;
            }
            tfp->next = fp->next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_destroy(fp->f_mutex);
        free(fp);
    } else {
        pthread_mutex_unlock(&hashlock);
    }
}


