    tasklet: 和软中断本质上很相似，但其接口更简单，锁保护要求较低，通常应该使用tasklet。


        1 tasklet的实现：
            tasklet通过软中断实现，本身也是软中断。由HI_SOFTIRQ和TASKLET_SOFTIRQ表示。

            （1）tasklet结构体：
                struct tasklet_struct {
                    struct tasklet_struct *next;    /* 链表中的下一个tasklet */
                    unsigned long state;            /* tasklet的状态 */
                    atomic_t count;                 /* 引用计数器 */
                    void (*func)(unsigned long );   /* tasklet处理函数 */
                    unsigned long data;             /* 给tasklet处理函数的参数 */
                }
                
         	（2）调度tasklet
         		已被调度的tasklet(等同于被触发的软中断)存放在两个单处理器数据结构tasklet_vec（普通tasklet）和tasklet_hi_vec（高优先级的tasklet）
         		两个都是由tasklet_struct 结构构成的链表，每个tasklet_struct 代表一个不同的tasklet.
         		由两个函数tasklet_schedule()和tasklet_hi_schedule()函数进行调度。
         		tasklet_schedule():
         			
         			a. 检查tasklet的状态，如果是TASKLET_STATE_SCHED，则表示tasklet已经被调度，立即返回
         			b. 调用_tasklet_schedule()
         			c. 保存中断状态，禁止本地中断（保证处理tasklet时，处理器上的数据不会乱）
         			d. 把需要调度的tasklet加到每个处理器上的tasklet_vec链表的表头
         			e. 唤起TASKLET_SOFTIRQ或HI_SOFTIRQ软中断，在下次do_softirq()时就会执行该tasklet
         			f. 恢复中断到原始状态
         			
         			
         			在do_softirq中会调用软中断处理程序，tasklet_action()和tasklet_hi_action():
         				a. 禁止中断，并为当前处理器检索tasklet_vec或tasklet_hig_vec链表
         				b. 将当前处理器上的链表设置为NULL。
         				c. 允许相应中断
         				d. 循环遍历获得链表上的每一个待处理的tasklet
         				e. 对于多处理器系统，通过检查TASKLET_STATE_RUN判断tasklet是否正在其他处理器上进行，如果正在运行，跳转到下一个
         				f. 当前tasklet没有执行，将state设置为TASKLET_STATE_RUN,
         				g. 检查count是否为0
         				h. 执行tasklet处理程序
         				i. 运行完毕，清除标志
         				j. 重复执行下一个tasklet，直至没有剩余等待处理的tasklet
         	
         	2 使用tasklet:
         		(1) 声明:
         			静态创建：
         				DECLARE_TASKLET(name, func, data); /* 引用记数为0，处于激活状态 */
         				DECLARE_TASKLET_DISABLED(name, func, data);		/* 引用计数1，禁止状态 */

					  间接引用初始化:
					  	void tasklet_init(t, tasklet_hadnler, dev);
					  	
					  	
				(2) 编写tasklet处理程序：
					void tasklet_handler(unsigned long data);
					注：因为靠软中断实现，所以tasklet不能睡眠，不能使用信号量或者其他阻塞式的函数。tasklet运行时允许相应中断，
						当与中断处理程序或者其他tasklet，必须适当的加锁保护
						
						
				（3）调度自己的tasklet
					tasklet_schedule(struct tasklet_struct *t);
					一个tasklet总在调度它的处理器上执行。
					
					禁止tasklet:
						tasklet_disable(struct tasklet_struct *t);
						如果已经运行，等待执行完毕返回，
					
					激活一个tasklet:
						tasklet_enable()
					    如果希望激活DECLARE_TASKLET_DISABLED()创建的tasklet,也得调用：
					    tasklet_disable(*my_tasklet)->tasklet_enable(&my_tasklet);
					    
					    
				(4) ksoftirqd:
						避免软中断的高频触发问题，引入内核线程辅助实施软中断执行工作：
						for( ; ; ) {
							if(!softirq_pending(cpu))
								schedule();
								
							set_current_state(TASK_RUNNING);
							
							while(softirq_pending(cpu)) {
								do_softirq();
								if(need_resched())
									schedule();
							}
							
							set_current_state(TASK_INTERRUPTIBLE);
						}			
         				
         				
         				
         				
      
