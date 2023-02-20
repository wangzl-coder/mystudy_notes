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
    唤醒：
        通过wake_up()函数进行，唤醒指定的等待在队列上的所有进程，其调用try_to_wake_up(),负责将进程设置为TASK_RUNNING状态
        调用enqueue_task()将进程放入红黑树，一般在促使等待条件达成的程序中调用wake_up()。注意虚假唤醒问题


#endif
