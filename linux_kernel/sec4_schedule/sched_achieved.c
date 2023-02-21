#if 0
    Linux 调度的实现（kernel/sched_fair.c）：

        1 时间记账：调度器对进程运行时间做记账
            （1）CFS调度器实体结构（<linux/shed.h>）：
                    struct sched_entity {
                        struct load_weight  load;
                        struct rb_node      run_node;
                        struct list_head    group_node;
                        unsigned int        on_rq;
                        u64                 exec_start;
                        u64                 sum_exec_runtime;
                        u64                 vruntime;
                        u64                 prev_sum_exec_runtime;
                        u64                 last_wakeup;
                        u64                 avg_overlap;
                        u64                 nr_migrations;
                        u64                 start_runtime;
                        u64                 avg_wakeup;
                    };
                其作为se的成员变量嵌入在struct task_struct结构中。

            （2）虚拟实时(vruntime)：vruntime存放进程运行虚拟时间(ns)，
#endif
/*
 *程序运行记账（kernel/sched_fair.c）
 *
 * */
static void update_curr(struct cfs_)
