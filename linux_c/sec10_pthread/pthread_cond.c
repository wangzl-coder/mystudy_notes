#if 0
    1 条件变量：另一种同步机制，与互斥量一起使用时，允许线程以无竞争方式等待特定条件的发生
        条件变量本身由互斥量保护，线程在改变条件状态之前必须先锁住互斥量，其他线程在获得互斥量之前
        不会察觉到这种改变，因为互斥量必须在锁定之后才能计算条件

    2 条件变量的初始化与释放：
        动态初始化：int phread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr)
        静态初始化：PTHREAD_COND_INITIALIZER
        释放：int pthread_cond_destroy(pthread_cond_t *cond);
        @return : 0 for success Or error number

    3 等待条件变量为真：
        int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
        int pthread_cond_timedwait(pthread_cond_t *restrict cond, phtread_mutex *restrict mutex,
                                        const struct timespec *restrict tsptr);

        传递的mutex对条件变量进行保护，调用者将已经加锁的互斥量传给函数，函数自动把调用线程放到等待
        条件的线程列表上，对互斥量解锁，返回时，互斥量再次被锁住。（为了不错过任何条件的变化）

        从两个函数成功返回时，线程需要重新计算条件，因为另一个线程可能已经运行并且改变了条件！！

    4 通知线程条件满足：
        int pthread_cond_signal(pthread_cond_t *cond);  //至少唤醒一个等待该条件的线程
        int pthread_cond_broadcast(pthread_cond_t *cond);   //唤醒等待该条件的所有线程
        @return : 0 for success or error number
        注：两个函数是在给程序或者条件发信号，所以一定要在改变条件后调用
#endif

        /* demo：工作队列 */
struct msg{
    struct msg *next;
    /* more stuff */
};

struct msg *workq;

pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;

void msg_handler(void)
{
    struct msg *pmsg;
        
    for(;;) {
        pthread_mutex_lock(&q_lock);
        while(workq == NULL)
            pthread_cond_wait(&qready);
        pmsg = workq;
        workq = workq->next;
        pthread_mutex_unlock(&q_lock);
        /* then handler process */
    }
}

void msg_send(struct msg *pmsg)
{
    pthread_mutex_lock(&q_lock);
    pmsg->next = workq;
    workq = pmsg;
    pthread_mutex_unlock(&q_lock);
    pthread_cond_signal(&qready);
}

