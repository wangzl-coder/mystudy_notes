#if 0
	软中断：
	
		1 软中断的实现：在编译期间静态分配，不像tasklet能被动态注册或注销，软中断由softirq_action结构表示
			struct softirq_action {
				void (*action){struct softirq_action *};
			};
			kernel/softirq.c中定义了一个包含有32个该结构体的数组：
			static struct softirq_action softirq_vec[NR_SOFTIRQS];
			每个被注册的软中断都占据该数组的一项，因此最后可能有32个软中断（定值，没法动态改变）
			
			（1）软中断处理程序：
				void softirq_handler(struct softirq_action *action);
				@action: 指向相应softirq_action结构体的指针，
				my_softirq->action(my_softirq);
				
				一个软中断不会抢占另一个软中断，实际上，唯一可以抢占软中断的是中断处理程序
				
			（2）执行软中断：一个软中断必须在被标记后才执行，这被称做触发软中断(raising the softirq)。通常
			中断处理程序会在返回前标记它的软中断，使其在稍后被执行。待处理的软中断被检查和执行的时机：
				a 从一个硬件中断代码返回时
				b 从ksoftirqd内核线程中
				c 从那些显式检查和执行处理的软中断代码中，如网络子系统中
				
			核心部分：do_softirq():
				u32 pending;
				
				pending = local_softirq_pending();
				if(pending) {
					struct softirq_action *h;
					
					/* 重设待处理的位图 */
					set_softirq_pending(0);
					h = softirq_vec;
					do {
						if(pending & 1)
							h->action(h);
						h++;
						pending >>= 1;
					} while(pending);
				}
				
		2 使用软中断：软中断保留给系统中对时间要求最严格以及最严重的下半部使用。目前，只有两个子系统网络和SCSI直接使用软中断，此外，
		内核定时器和tasklet都是建立在软中断上的。
		
			（1）分配索引：内核用索引表示一种相对优先级，索引号小的软中断在索引号大的软中断之前执行。
			（2）注册处理程序：
			
#endif

