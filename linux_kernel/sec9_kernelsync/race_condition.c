#if 0
    临界区和竞争条件

        1 临界区和竞争条件：
         	临界区：访问和操作系统共享数据的代码段，
         	（1）为什么需要保护：
         	某些事务的原子性，这样的事必须完整的发生，
         	（2）单个变量：
         
         
         2 加锁：
         	
         		（1）造成并发执行的原因（内核）：
         		a. 中断，中断几乎可以在任何时刻异步发生，也就可能随时打断当前正在执行的代码
         		b. 软中断和tasklet:内核能在任何时刻唤醒或调度软中断和tasklet，打断当前正在执行的代码
         		c. 内核抢占: 内核具有抢占性，所以内核中的任务可能被另一任务抢占
         		d. 睡眠及与用户空间的同步: 在内核执行的进程可能会睡眠，这就会唤醒调度程序，从而导致一个新的用户进程执行
         		e. 对称多处理: 两个或多个处理器可以同时执行。
         		
         		（2）加锁的对象：共享数据而不是代码
     
         3 死锁:
         	死锁产生的条件： 一个执行线程的尝试去获得一个自己已经持有的锁； 多个线程对多个资源的交叉加锁
         
         4 争用和扩展性：
         	加锁的粒度：
         	
         			
#endif
