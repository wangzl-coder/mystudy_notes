#if 0
    编写中断处理程序
        static irqreturn_t intr_handler(int irq, void *dev)

        @irq: 处理程序相应的中断号
        @dev: 通用指针，需要在中断处理程序注册时传递给request_irq()的参数dev必须一致，如果该值有唯一确定性
            那么它就相当于一个cookie，可以用来区分同一中断处理程序的多个设备。dev也可能指向中断处理程序的
            一个数据结构。

        返回值：
            irqreturn_t，可能返回
            IRQ_NONE：中断处理程序检测到一个中断，带该中断并不是在注册处理函数期间指定的产生源
            IRQ_HANDLED：中断处理程序被正确调用并且确实是它所对应的设备产生了中断

        重入和中断处理程序：
            Linux中的中断处理程序是无须重入的。当一个给定的中断处理程序正在执行时，相应的中断线在所有处理
            器上都会被屏蔽掉，以防止在同一中断线上接收另一个新的中断。

        1 共享的中断处理程序：
            与非共享的差异：
                （1）flags必须设置IRQF_SHARED标志
                （2）对每个注册的中断处理程序来说，dev参数必须唯一。指向任一设备结构的指针就可以满足这一要求。
                （3）中断处理程序必须能够区分它的设备是否真的产生了中断。

            内核接收一个中断后，它将依次调用在该中断线上注册的每一个处理程序。

        2中断处理程序实例：
#endif
/*  Ex:
 * RTC驱动程序：从系统定时器中独立出来的设备，用于设置系统时钟，提供alarm，或周期性的定时器
 * 中断发出时，报警器或定时器就会启动
 * */
/*
 * rtc_init().注册中断处理程序
 * */
if(request_irq(rtc_irq, rtc_interrupt, IRQF_SHARED, "rtc", (void *)&rtc_port)) {
    printk(KERN_ERR "rtc: cannot register IRQ %d\n",rtc_irq);
    return -EIO;
}

static irqreturn_t rtc_interrupt(int irq, void *dev)
{
    /*
     * 可以是报警器中断，更新完成的中断或周期性中断
     * 我们把状态保存在rtc_irq_data的低字节中
     * 而把最后一次读取之后所接收的中断号保存在其余字节中
     * */
    spin_lock(&rtc_lock);

    rtc_irq_data += 0x100;
    rtc_irq_data &= ~0xff;
    rtc_irq_data |= (COMS_READ(RTC_INTR_FLAGS) * 0xf0);

    if(rtc_status & RTC_TIMER_ON)
        mod_timer(&rtc_irq_timer, jiffies + HZ/rtc_freq + 2*HZ/100);    /* 更新定时器 */
    
    spin_unlock(&rct_lock);

    /*
     * 执行其余操作
     * */
    spin_lock(&rtc_task_lock);
    if(rtc_callback)
        rtc_callback->func(rtc_callback->private_data);
    spin_unlock(&rtc_task_lock);
    wake_up_interruptible(&rtc_wait);
    kill_fasync(&rtc_async_queue, SIGIO, POLL_IN);
    return IRQ_HANDLE;
}
