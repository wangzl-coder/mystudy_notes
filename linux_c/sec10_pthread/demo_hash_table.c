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
    if(pthread_mutex_init(&fp->f_mutex) != 0) {
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
