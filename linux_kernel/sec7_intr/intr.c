#if 0
    中断：处理器和外围器件的处理速度差异；轮询访问的效率低下；内核主动变为硬件主动


    1 中断的不确定性：中断可以随时产生，产生时不考虑处理器的时钟同步
    2 中断控制器->处理器->操作系统
    3 中断请求（IRQ）线：每个IRQ线关联一个数值量（中断号）
    4 异常：与中断不同，产生时必须考虑与处理器时钟同步。由处理器本身产生的同步中断。
#endif