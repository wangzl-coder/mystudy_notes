#if 0
	原子操作：保证指令以原子的方式执行，执行过程不被打断。
	
	
		（1）原子整数操作：
			atomic_t :
			typedef struct {
				volatile int counter;
			} atomic_t;
				
			定义atomic_t 类型，并设定初值:
			acomic_t v;
			atomic_t u = ATOMIC_INIT(0); /* 定义u并把它初始化为0 */
			操作：
			atomic_set(&v, 4);	/* v = 4 */
			atomic_add(2, &v);	/* v = v + 2 = 6*/
			atomic_inc(&v);	/* v = v + 1 = 7 */
			atomic_dec(&v)		/* v = v - 1 = 6 */
			将atomic_t转换成int型，可以使用atomic_read()完成：
			printk("%d\n", atomic_read(&v));	/* 打印 6 */
			...
			
		（2）64位原子操作：
			typedef struct {
				volatile long counter;
			} atomic64_t;
			
		（3）原子位操作： 是对普通的内存地址进行操作的，他的参数是一个指针和一个位号，其对位号的范围并没有限制：
			unsigned long word = 0;
			
			set_bit(0, &word);	/* 第0位被设置 */
			set_bit(1, &word);	/* 第1位被设置 */
			printk("%ul\n", word);	/* 3 */
			clear_bit(1, &word);	/* 清空第1位 */
			change_bit(0, &word);	/* 翻转第0位的值 */
			
			
#endif
