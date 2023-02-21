#if 0
    Linux调度算法：
        
        1 调度器类
            （1）Linux调度器以模块化方式提供，允许不同类型的进程可以针对性的选择调度算法。模块化结构称为调度器类，
            （2）完全公平调度（CFS）：针对普通进程的调度类，SCHE_NORMAL,

        2 Unix系统中的进程调度
            （1）将nice值映射到时间片的不合理性：nice单位值对应到处理器的绝对时间，导致进程切换无法优化进行，（举例：两个相同
                nice值的低优先级进程会高频的上下文切换；高nice值的后台进程将占用更多时间片，低nice值得前台用户任务将获得很少的时间片）

            （2）两个进程间绝对nice值的不合理性，相对nice值

            （3）nice值到时间片的映射，导致最小时间片需是定时器节拍的整数倍，并会随定时器节拍改变

            （4）新唤醒进程提高优先级的策略将使得给定进程打破公平原则。

            解决策略：
            （1）nice值的几何增加。
            （2）新的度量机制将nice值到时间片的映射与定时器节拍分离

            依然存在的根本问题：分配绝对的时间片引发的固定切换频率，给公平性造成很大变数

            CFS算法：根本性的重新设计，完全摒弃时间片二十分配给进程处理器使用的比重。

        3 CFS公平调度：
            （1）允许每个进程运行一段时间，循环轮转，选择运行最少的进程作为下一个运行进程。使用所有进程对处理器的使用权重作为调度参考
            （2）进程nice越低，获得处理器使用权重越高，nice值越高，获得处理器使用权重越低。
            
            总结：任何进程所获得的处理器时间是由它自己和其他所有可运行进程的nice值的相对差值决定，nice值对时间片的作用不再是算数加权，
                而是几何加权，任何nice值对应的绝对时间不再是绝对值，而是处理器的使用比，CFS确保给每个进程公平的处理器使用比。
#endif
/*
 * 系统定时器周期性调用
 * */
static void update_curr(struct cfs_rq *cfs_rq)
{
    struct sched_entity *curr = cfs_rq->curr;
    u64 now = rq_of(cfs_rq)->clock;
    unsigned long delta_exec;

    if(unlikely(!curr))
        return;

    /* 获取最后一次修改负载后当前进程运行的时间 */
    delta_exec = (unsigned long) (now - curr->exec_start);
    if(!delta_exec)
        return;
    
    /*
     * 根据进程总数对运行时间加权计算，并与vruntime相加
     * */
    __update_curr(cfs_rq, curr, delta_exec);
    curr->exec_start = now;

    if(entity_is_task(curr)) {
        struct task_struct *curtask = task_of(curr);
        trace_sched_stat_runtime(curtask, delta_exec, curr->vruntime);
        cpuacct_charge(curtask, delta_exec);
        account_group_exec_runtime(curtask, delta_exec);
    }
}

static inline void __update_curr(struct cfs_rq *cfs_rq, struct sched_entity *curr, unsigned long delta_exec)
{
    unsigned long delta_exec_weighted;

    schedstat_set(curr->exec_max, max((u64)delta_exec, curr->exec_max));
    curr->sum_exec_runtime += delta_exec;   /* 进程总运行时间 */
    /* 计算权重 */
    schedstat_add(cfs_rq, exec_clock, delta_exec);  
    delta_exec_weighted = calc_delta_fair(delta_exec, curr):;   

    curr->vruntime += delta_exec_weighted;      /* 得到vruntime */
    update_min_vruntime(cfs_rq);
}


#if 0
    进程选择：CFS使用红黑树组织可运行进程队列，并选择vruntime最小的进程作为下一个执行进程

        （1）挑选下一个任务：将可运行程序的虚拟时间作为键值，则沿着树左侧一直找到子节点便可以得到vruntime最小的进程：

        （2）向树中加入进程
#endif
static struct sched_entity *__pick_next_entity(struct cfs_rq *cfs_rq)
{
    /*
     * 实际上不需要遍历树，当前值已经缓存在rb_leftmost中
     **/
    struct rb_node *left = cfs_rq->rb_leftmost;

    if(!left)
        return NULL;

    return rb_entry(left, struct sched_entity, run_node);
}

/*
 * 发生在进程被唤醒或通过fork调用第一次创建进程时
 * */
static void enqueue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se, int flags)
{
    /*
     * 通过调用update_curr(),在更新min_vruntime之前先更新规范化的vruntime
     * */
    if(!(flags & ENQUEUE_WAKEUP) || (flags & ENQUEUE_MIGRATE))
        se_vruntime += cfs_rq->min_vruntime;

    /* 更新当前任务的运行时统计信息 */
    update_curr(cfs_rq);
    
    account_entity_enqueue(cfs_rq, se);

    if(flags & ENQUEUE_WAKEUP) {
        palce_entity(cfs_rq, se, 0);
        enqueue_sleeper(cfs_rq, se);
    }
    
    update_stats_enqueue(cfs_rq, se);
    check_spread(cfs_rq, se);
    if(se != cfs_rq->curr)
        __enqueue_entity(cfs_rq, se);   /* 更新完运行时间和其他统计信息后，插入树 */
}

/*
 * 把一个调度实体插入红黑树中
 * */
static void __enqueue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se);
{
    struct rb_node **link = &cfs_rq->tasks_timeline.rb_node;
    struct rb_node *parent = NULL;
    struct sched_entity *entry;
    s64 key = entity_key(cfs_rq, se);
    int leftmost = 1;

    /* 在红黑树中查找合适位置 */
    while(*link) {
        parent = *link;
        entry = rb_entry(parent, struct sched_entity, run_node);

        /* 并不关心冲突，具有相同的键值呆在一起 */
        if(key < entity_key(cfs_rq, entry))
            link = &parent->rb_left;
        else {
            link = &parent->rb_right;
            leftmost = 1;
        }
    }

    /* 维护一个缓存，存放树最左子叶节点 */
    if(leftmost)
        cfs_rq->rb_leftmost = &se->runnode;

    rb_link_node(&se->run_node, parent, link);  /* 循环结束，将当前进程置为当前父节点的子节点 */
    rb_insert_color(&se->run_node, &cfs_rq->tasks_timeline);    /* 更新树的自平衡相关属性 */
}

/*
 * 从树中删除进程
 * */
static void dequeue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se, int sleep)
{
    /* 同理更新当前任务运行时统计数据 */
    update_curr(cfs_rq);

    update_stats_dequeue(cfs_rq, se);
    clear_buddies(cfs_rq, se);

    if(se != cfs_rq->curr)
        __dequeue_entity(cfs_rq, se);
    account_entity_dequeue(cfs_rq, se);
    update_min_vruntime(cfs_rq);

    if(!sleep)
        se->vruntime -= cfs_rq->min_vruntime;
}

static void __dequeue_entity(struct cfs_rq *cfs_rq, srtuct sched_entity *se)
{
    if(cfs_rq->rb_leftmost == &se->run_node) {
        struct rb_node *next_node;

        next_node = rb_next(&se->run_node); /* 找到下一个最左侧节点 */
        cfs_rq->rb_leftmost = next_node;
    }
    rb_erase(&se->run_node, &cfs_rq->tasks_timeline);
}

#if 0
    3 调度器入口：schedule();
        (1) 找到最高优先级的调度类，每个调度类维护可运行队列，然后找到下一个该运行的进程
#endif

/*
 * 以优先级为序，从高到低依次检查每一个调度类，并从最高优先级的调度类中，选择最高优先级的进程
 * */
static inline struct task_struct *pick_next_task(struct rq *rq)
{
    const struct sched_class *class;
    struct task_struct *p;

    /*
     * 优化，如果所有任务都在公平类中，则可以直接调用
     * sched_class 的pick_next_task调用pick_next_entity(),后者调用__pick_next_entity()找到当前类的leftmost进程
     * */
    if(likely(rq->nr_running == rq->cfs.nr_running)) {
        p = fair_sched_class.pick_next_task(rq);
        if((likely(p)))
            return p;
    }
    class = sched_class_highest;
    for( ; ; ) {
        p = class->pick_next_task(rq);
        if(p)
            return p;

        class = class->next;
}

#if 0
    5 睡眠和唤醒：IO操作，信号量等待等情况下，进程把自己标记成休眠状态，从可执行红黑树中移出，放入等待队列，然后调用
        schedule()选择和执行一个其他进程。唤醒过程刚好相反，进程被设置为可执行状态，然后从等待队列移到可执行红黑树中。
        TASK_INTERRUPTIBLE和TASK_UNINTERRUPTIBLE
    
        (1) 等待队列：等待事件发生的进程组成的简单链表，内核表示：wake_queue_head_t,
            a. 等待队列初始化：静态创建DECLARE_WAITQUEUE(),动态创建init_waitqueue_head();
            b. 注意问题：休眠和唤醒的竞争条件，
                在内核中进行休眠的推荐操作：
                /* q 是我们希望休眠的等待队列 */
                DEFINE_WAIT(wait);

                add_wait_queue(q, &wait);   /* 将自己加入到等待队列，并等待wake_up操作将其唤醒 */
                while(!condition){  /* condition 等待条件，严格点需要原子操作 */
                    prepare_to_wait(&q, &wait, TASK_INTERRUPTIBLE); /* 设定状态，并可能继续加入到等待队列 */
                    if(signal_pending(current)) /* 可能被信号打断，伪唤醒，信号处理 */
                        /* 信号处理 */
                    schedule();     /* 调度下一个可执行进程 */
                }
                finish_wait(&q, &wait); /* 条件达成，状态设置为TASK_RUNNING,并移出等待队列 */

        (2) 唤醒:
                通过wake_up()函数进行，唤醒指定的等待在队列上的所有进程，其调用try_to_wake_up(),负责将进程设置为TASK_RUNNING状态
            调用enqueue_task()将进程放入红黑树，一般在促使等待条件达成的程序中调用wake_up()。注意虚假唤醒问题
#endif
/*
 *  example for wait_queue，
 *  /fs/notify/inotify/inotify_use.c，负责通知从文件描述符中读取信息
 *
 * */
static ssize_t inotify_read(struct filie *file, char __user *buf, size_t count, loff_t *pos)
{
    struct fsnotify_group *group;
    struct fsnotify_event *kevent;
    char __user *start;
    int ret;
    DAFINE_WAIT(wait);

    start = buf;
    group = file->private_data;

    while(1) {
        prepare_to_wait(&group->notification_waitq, &wait, ,TASK_INTERRUPTIBLE);

        mutex_lock(&group->notification_mutex);
        kevent = get_one_event(group, count);
        mutex_unlock(&group->notification_mutex);

        if(kevent) {
            ret = PTR_ERR(kevent);
            if(IS_ERR(kevent))
                break;
            ret = copy_event_to_user(group, kevent, buf);
            fsnotify_put_event(kevent);
            if(ret < 0)
                break;
            buf += ret;
            count -= ret;
            continue;
        }
        ret = -EAGAIN;
        if(file->flags & O_NONBLOCK)
            break;
        ret = -EINTR;
        if(signal_pending(current))
            break;

        if(start != buf)
            break;

        schedule();
    }
    finish_wait(&group->notificationq, &wait);

    if(start != buf && ret != -EFAULT)
        ret = buf - start;
    return ret;
}
