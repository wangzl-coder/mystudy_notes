#if 0
    中断控制：
        通过禁止中断，可以确保某个中断处理程序不会抢占当前的代码，禁止中断还可以禁止内核抢占，
        但是无法提供保护机制来防止来自其他处理器的并发访问。

        1 禁止和激活中断：
            /* 禁止当前处理器的本地中断 */
            local_irq_disable();
            /* 激活当前处理器的本地中断 */
            local_irq_enable();

            潜在危险：复杂的调用链将可能导致当前的激活不符合以前的禁止操作，其将会激活以前可能已经禁止的中断，
            所以，禁止中断时需要保存以前的状态，而后需要恢复：
            unsigned long flags;
            local_irq_save(flags);  /* 禁止中断 */
            /* ... */
            local_irq_restore(flags);    /* 中断恢复到原来的状态 */
            注：flags不能传递给另一个函数，两个函数的调用必须在同一个函数内完成

        2 禁止指定中断线：
            void disable_irq(unsigned int irq);
            /* 禁止中断控制器上指定的中断线，当前正在执行的所有处理程序完成后，才能返回，
             * 需要保证不在指定线上传递新的中断，同时确保所有已经开始执行的处理程序已全部退出。
             * */
            void disable_irq_nosync(unsigned int irq);
            /* 不会等待当前中断处理程序执行完毕 */

            void enable_irq(unsigned int irq);
            /* 激活指定中断线，对于禁止的每次嵌套操作，都要由相应次数的激活才能真正激活中断线 */
            
            void synchronize_irq(unsigned int irq);
            /* 等待一个特定的中断处理程序的退出 */
            
            注：三个激活或禁止函数可以从中断或进程上下文调用，而且不会睡眠，但如果从中断上下文调用
            需要特别小心，例如，当正在处理一条中断线时，并不想激活它（当某个处理程序的中断线正在被
            处理时，他被屏蔽掉）；

            禁止多个中断处理程序共享的中断线是不合适的。


        3 中断系统的状态：

            /* 获取本地处理器上的中断系统是否被禁止  */
            irqs_disable();

            /* 检查内核当前上下文环境 */
            in_interrupt(); /* 内核处于任何类型的中断处理中 */
            int_irq();      /* 内核确实正在执行中断处理 */
#endif
