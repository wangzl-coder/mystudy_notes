#if 0
	信号量：一种睡眠锁，...
	
		. 由于等待信号量时会等待睡眠，信号量适用于锁会被长时间持有的情况
		. 持有时间段短时，信号量不适用，睡眠，维护等待队列，唤醒开销比占用锁的时间还长
		. 只能在进程上下文获取，中断上下文中不行
		. 在持有信号量时可以去睡眠，
		. 占用信号量的同时不能占用自旋锁，因为等待信号量时可能会睡眠，但在持有自旋锁时是不允许睡眠的。
		
		1 计数信号量与二值信号量
			
			计数信号量: 允许在同一时刻至多有count个锁持有者，不能用来进行强制互斥，因为其允许多个执行线程
				同时访问临界区。
			二值信号量： 互斥信号量
			
			P( down() ), V( up() )操作：
			
		2 创建和初始化信号量： sema_init(struct semaphore *sem, int val);
			struct semaphore name;
			sema_init(&name, count);
			
			创建互斥信号量：
			static DECLARE_MUTEX(name);
		
		3 使用信号量：
			down(struct semaphore *sem);
			up(struct semaphore *sem);
			down_interruptible(struct semaphore *);
			down_trylock(struct semaphore *);
			
		4 读 - 写信号量：所有的读写锁睡眠都不会被信号打断（只有down操作）
			创建读写信号量：
				init_rwsem(struct rw_semaphore *sem);
				static DECLARE_RWSEM(mr_rwsem);
			
			信号量操作：
				down_read(&mr_rwsem);
				
				/* 临界区，只读 ... */
				
				/* 释放信号量 */
				up_read(&mr_rwsem);
				
				/* 试图获取信号量用于写 */
				down_write(&mr_rwsem);	
				
				/* 临界区，读和写 ... */
				
				up_write(&mr_rwsem);
			其他：
				down_read_trylock();
				down_write_trylock();
				@return : 与普通信号量相反，信号量被争用，返回0，获取成功返回非0；
				
				
					
#endif
