#if 0
    中断处理机制的实现：依赖于体系结构，

    中断流程：
        设备产生中断->中断控制器，如果中断被激活->处理器->停止正在做的事，关闭中断系统，跳转预定义入口：
            对于每条中断，处理器都会跳转到对应的一个唯一的位置。初始入口点在栈中保存中断号，并存放当前
            寄存器的值，然后调用do_IRQ();
        unsigned int do_IRQ(struct pt_regs regs);
        do_IRQ计算出中断号，对所接受的中断进行应答，禁止这条线上的中断。
        调用handle_IRQ_event运行已经安装的中断处理程序
        返回到do_IRQ(),清理工作并返回到初始入口点，从入口点跳转ret_from_intr();
        ret_from_intr:
            检查重新调度是否正在挂起(sec4: need_resched),如果正在挂起：
                a. 内核正在返回用户空间(中断了用户进程)，schedule()被调用
                b. 内核正在返回内核空间(中断了内核本身)，只有在preempt_count
                为0时，schedule()才会被调用，否则，抢占内核是不安全的


#endif
/*
 * handle_IRQ_event - irq action chain handler
 * @irq: the interrupt number
 * @action : the interrupt action chain for this irq
 *
 * Handles the action chain of an irq event
 * */
irqreturn_t handle_IRQ_event(unsigned int irq, struct irqacion *action)
{
    irqreturn_t ret, retval = IRQ_NONE;
    unsigned int status = 0;
    
    /*
     * 处理器禁止了中断，这里要检查并打开
     * */
    if(!(action->flags & IRQF_DISABLE))
        local_irq_enable_in_hardirq();

    do {    /* 对所有共享中断线的处理程序遍历 */
        trace_irq_handler_entry(irq, action);
        ret = action->handler(irq, action->dev_id);
        trace_irq_handler_exit(irq, action, ret);

        switch(ret) {
            case IRQ_WAKE_THREAD:
                /*
                 *把返回值设置为已处理，以便可疑的检查不再触发
                 * */
                ret = IRQ_HANDLED;
                /*
                 * 捕获返回值为WAKE_THREAD的驱动程序，但是并不创建一个线程函数
                 * */
                if(unlikely(!action->thread_fn)) {
                    warn_no_thread(irq, action);
                    break;
                }
                /*
                 * 为这次中断唤醒处理线程，万一线程崩溃且被杀死，我们仅仅假装已经处理了该中断
                 * 上述的硬件中断hardirq处理程序已经禁止设备中断，因此杜绝irq产生
                 * */
                if(likely(!test_bit(IRQTF_DIED, &action->thread_flags))) {
                    set_bit(IRQF_RUNTHREAD, &action->thread_flags);
                    wake_up_process(action->thread);
                }

                /* Fall through to add to randomness */
            case IRQ_HANDLED:
                status |= action->flags;
                break;
            default:
                break;
        }
        retval |= ret;
        action = action->next;
    } while(action)
    
    /* 注册时指定标志，将中断间隔时间为随机数产生器产生熵 */
    if(status & IRQF_SAMPLE_RANDOM)
        add_interrupt_randomness(irq);
    /* 禁止中断 */
    local_irq_disable();
    return retval;
}
