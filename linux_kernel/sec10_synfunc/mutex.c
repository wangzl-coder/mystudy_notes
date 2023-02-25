#if 0
	互斥体：可以睡眠的强制互斥锁，相当于计数为1的信号量。
		
		初始化：
			DEIFNE_MUTEX(name);
			struct mutex mutex;
			mutex_init(&mutex);
			
		对互斥体锁定和解锁：
			mutex_lock(&mutex);
			mutex_unlock(&mutex);
			
			mutex_trylock(&mutex);
			mutex_is_locked(struct mutex *);
			
		mutex使用场景：
			. 任何时刻中只有一个任务可以持有mutex, 也就是说，mutex计数永远是1
			. 给mutex上锁者必须负责给其再解锁，最常用的方式：在同一上下文中上锁和解锁
			. 递归上锁和解锁是不允许的
			. 当持有一个mutex时，进程不可以推出
			. mutex不能在中断或者下半部中使用, mutex_trylock()也不行
			. mutex只能通过官方API管理
		
		1 信号量和互斥体：
			优先选择mutex，不能满足约束条件，且没有其他选择时，再考虑信号量
			
		2 自旋锁和互斥体：
			中断上下文中只能使用自旋锁，而在任务睡眠时只能使用互斥体
		
			低开销加锁 -> 优先使用自旋锁
			短期锁定 -> 优先使用自旋锁
			长期加锁 -> 优先使用互斥体
			中断上下文加锁 -> 使用自旋锁
			持有锁需要睡眠 -> 使用互斥体
#endif
