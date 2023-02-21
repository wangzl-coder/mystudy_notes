#if 0
    注册中断处理程序
        
        int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev);
        @irq:中断号，可通过探测获取或编程动态确定
        @handler：指向处理中断的实际中断处理程序的指针，typedef irqreturn_t (*irq_handler_t)(int, void *)
        @flags: 中断处理程序标志
        @name: 与中断相关的设备的ASCII码表示
        @dev: 用于共享中断线，当一个中断处理程序需要释放时，dev将提供唯一的标志信息，以便从共享中断线的诸多中断处理
        程序中删除指定的那一个。

        @return: 0 for success, or error.

        注意：request_irq()函数可能会睡眠，因此，不能在中断上下文或者其他不允许阻塞的代码中调用该函数。在睡眠不安全的
        上下文中调用request_irq()是一种常见错误。

        1 中断处理程序标志：可以为0，也可以时一个或多个标志的位掩码
            IRQF_DISABLED: 处理中断处理程序期间，禁止所有的其他中断。留给希望快速执行的轻量级中断

            IRQF_SAMPLE_RANDOM: 产生的中断时间间隔将贡献到内核熵池(entropy pool)

            IRQF_TIMER: 为系统定时器的中断准备

            IRQF_SHARED: 可以在多个中断处理程序之间共享中断线，

        2 中断例子
            if(request_irq(irqn, my_interrupt, IRQF_SHARED, "my_device", my_dev)) {
                printk(KERN_ERR "my_device: cannot register IRQ %d\n", irqn);
                return -EIO;
            }
        
        3 释放中断处理程序：卸载驱动程序时，需要注销相应的中断处理程序，并释放中断线：
            void free_irq(unsigned int irq, void *dev);
            如果中断线不共享，将同时禁用这条中断线；如果共享，则仅删除dev所对应的处理程序，而这条中断线本身只有
            在删除了最后一个处理程序时才会被禁用。
#endif
