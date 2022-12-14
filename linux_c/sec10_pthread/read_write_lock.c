#if 0
    1 读写锁：也叫共享互斥锁（shared-exclusive lock）机制：
        （1）一次只能有一个线程占用写模式锁，可有多个线程占有读模式锁
        （2）当写加锁时，所有的加锁尝试都被阻塞
        （3）当读加锁时，获取读模式锁的线程可以加锁，尝试写加锁时将被阻塞等待所有读加锁
                释放，当获取写加锁后，后续的读加锁将被阻塞，用以避免写锁长期等待
        （4）适用于读数据次数远大于写次数的情况
        （5）与互斥量相比，读写锁使用前必须初始化，在其释放底层内存前必须销毁

    int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                            const ptread_rwlockattr_t *restrict attr);

    int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
    @return : 0 for success , or return error number
    静态初始化：PTHREAD_RWLOCK_INITIALIZER（attr默认）
    
    2 读加锁
        int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
      
      写加锁
        int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

      释放锁：
        int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

        @return : 0 for success or error number

     3 非阻塞获取
        int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
        int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
        @return : 0 for success or error numbebr(EBUSY ..)
    
    4 带超时的读写锁
        int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict tsptr);
        int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict tsptr);
        @return : 0 for success Or error number(ETIMEDOUT)
#endif

                    /* demo: 作业请求队列，单个线程为多个线程分配作业，队列使用单个读写锁保护 */
typedef struct job_st {
    struct job_st *next;
    struct job_st *prev;
    pthread_t j_id;
    /* more stuff */
}job;

typedef struct job_queue_st{
    struct job_st *j_head;
    struct job_st *h_tail;
    pthread_rwlock_t j_lock;
}job_queue;

int job_queue_init(job_queue *queue)
{
    queue->jhead = NULL;
    queue->htail = NULL;
    return (pthread_rwlock_init(&queue->j_lock, NULL));
}

/* insert job to head of queue */
int job_insert(job_queue *queue, job *jp)
{
    pthread_rwlock_wrlock(&queue->j_lock);
    jp->next = queue->j_head;
    jp->prev = NULL;
    if(queue->j_head == NULL) {
        queue->tail = jb;
    } else {
        queue->head->prev = jb;
    }
    queue->head = jb;
    pthread_rwlock_unlock(&queue->j_lock);
    return 0;
}

/* insert job to tail of queue */
int job_append(job_queue *queue, job *jp)
{
    pthread_rwlock_wrlock(&queue->j_lock);
    jp->next = NULL;
    jp->prev = queue->tail;
    if(queue->j_head == NULL) {
        queue->j_head = jp;
    } else {
        queue->j_tail->next = jp;
    }
    queue->j_tail = jp;
    pthread_rwlock_unlock(&queue->j_lock);
    return 0;
}

/* remove job from queue */
void job_remove(job_queue *queue, job *jp)
{
    pthread_rwlock_wrlock(&queue->j_lock);
    if(queue->j_head == jp) {
        queue->j_head = jp->next;
        if(queue->j_tail == jp) /* only jop in the queue */
            queue->j_tail = NULL;
        else
            jp->next->prev = jp->prev;  /* in fact, it's NULL */
    } else if(queue->j_tail == jp) {
        jp->prev->next = jp->next;      /* in fact, it's NULL */
        queue->j_tail = jp->prev;
    } else {
        jp->prev->next = jp->next;
        jp->next->prev = jp->prev;
    }    
    pthread_rwlock_unlock(&queue->j_lock);
}

/* find job by thread id from queue */
struct jop *jop_find(job_queue *queue, pthread_t id)
{
    struct jop *jp;
    pthread_rwlock_rdlock(queue->j_lock);
    for(jp = queue->head; jp != NULL; jp = jp->next) {
        if(pthread_equal(jp->j_id, id))
            break;
    }
    pthread_rwlock_unlock(queue->j_lock);
    return jp;
}
