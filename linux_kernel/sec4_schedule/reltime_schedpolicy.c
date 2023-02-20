#if 0
    实时调度策略:
        普通，非实时调度策略：SCHED_NORMAL.

        实时调度策略：SCHED_FIFO, SHCED_RR

        SCHED_FIFO：实现一种简单的，先入先出的调度算法。SCHED_FIFO进程比任何SCHED_NORMAL极的进程都先
            得到调度，其不使用时间片，得到调度后会一直执行，知道它自己受阻塞或者显式的释放处理器。

        SCHED_RR：SCHED_RR级的进程是带有时间片的SCHED_RR，其耗尽事先分配给它的时间后就不能再继续执行
#endif  
