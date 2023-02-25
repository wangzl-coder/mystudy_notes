#if 0
	自旋锁：执行线程试图获得一个被已经持有的自选锁，那么该i线程就会一直进行忙循环-旋转-等待锁重新可用
		自旋锁不会引起睡眠，但浪费处理器时间，适用于短期时间内的轻量级加锁以及在中断上下文中使用
		
		1 自旋锁方法：
			基本使用形式：
			静态创建初始化：
			DEFINE_SPINLOCK(mrlock);
			动态地址初始化：
			spinlock_t mrlock;
			spin_lock_init(&mrlock);
			加锁：
			spin_lock(&mrlock);
			释放锁：
			spin_lock(&mrlock);
		
			注意：
				（1）自旋锁是不可递归的，否则会出现死锁
				（2）自选锁可以在中断处理程序中使用，在中断处理程序中有使用自旋锁时，在其他代码获取该锁之前需要禁止本地中断（当前处理器的中断），
				否则，中断处理程序可能打断当前的内核代码，试图去获取锁，此时，也会产生死锁。	
				（3）持有自旋锁的情况下，不能睡眠，因为持有自旋锁时会禁止当前处理器的内核抢占(preempt_count)。
			禁止中断同时请求锁：
			DEFINE_SPINLOCK(mr_lock);
			unsigned long flags;
			spin_lock_irqsave(&mr_lock, flags);	/* 禁止本地中断并保存状态，获取锁 */
			/* 临界区... */
			spin_unlock_irqstore(&mr_lock, flags);	/* 释放锁并恢复中断到之前状态 */
			
		2 其他针对自旋锁的操作：
			spin_try_lock();
			尝试获取锁，该锁被争用之后，返回非0值。成功获取锁，返回0，
			
			spin_is_locked();
			检查特定的锁是否已经被争用，如果已经被占用，返回非0值。
			
			...
			
		3 自旋锁和下半部：
			如果进程上下文和下半部共享数据时，由于下半部可以抢占进程上下文中的代码，所以需要禁止下半部再获取锁
			spin_lock_bh();
			spin_unlock_bh();
			如果中断处理程序和下半部共享数据，那么就必须在获取恰当的锁的同时还要禁止中断
			
			同类的tasklet不可能同时运行，所以对于同类tasklet中的共享数据不需要保护，但是当数据被两个不同种类的tasklet共享时，就需要在访问
			下半部的数据前先获得一个普通的自旋索。这里不需要禁止下半部，因为在同一个处理器上绝不会有tasklet相互抢占的情况
			
			同类型的软中断可以同时运行在一个系统的多个处理器上，所以，无论是否同种类型，如果数据被软中断共享，那么它必须得到锁保护，但是同一
			处理器上的一个软中断绝不会抢占另一个软中断，因此，根本没必要禁止下半部。
			
			
			
		4 读--写自旋锁：
			一个或多个读任务可以并发的持有读者锁（共享锁）
			用于写的锁最多只能被一个写任务持有，而且此时不能有并发的读操作（排斥锁）。
			
			读写锁的使用方法：
			DEFINE_RWLOCK(mr_rwlock);
			
			read_lock(&mr_rwlock);
			/* 临界区 只读... */
			read_unlock(&mr_rwlock);
			
			
			write_lock(&mr_rwlock);
			/* 临界区，读写 */	
			write_unlock(&mr_rwlock);
			
			其他：
				read_lock_irq();
				read_lokc_irqsave();
				read_unlock();
				read_unlock_irq();
				read_unlokc_irqsrestore();
				wirte_lock_irq();
				write_lock_irqsave();
				write_unlock_irq();
				write_unlock_irqsave();
				
			考虑：大量的读者会使挂起的写者处于饥饿状态
			
			
#endif
