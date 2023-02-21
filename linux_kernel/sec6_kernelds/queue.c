#if 0
    队列：生产者消费者模型。先进先出，FIFO


        1 kfifo: enqueue入队，dequeue出队，入口偏移，出口偏移，出口偏移总是小于等于入口偏移

        2 创建队列：
            int kfifo_alloc(struct kfifo *fifo, unsigned int size, gfp_t gfp_mask)
            @fifo: 回填队列
            @size : 初始化大小
            @gfp_mask: 内核队列标识
            @return: 0 for success, or errno for error

            ex:
                struct kfifo fifo;
                int ret;

                /* 分配大小为PAGE_SIZE的队列 */
                ret = kfifo_alloc(&fifo, PAGE_SIZE, GFP_KERNEL);
                if(ret)
                    return ret;

                自己分配缓冲区：
                void kfifo_init(struct kfifo *fifo, void *buffer, unsigned int size);
                /* 创建并初始化一个kfifo对象，其将由buffer指向的size字节大小的内存， */

                静态声明：
                DECLARE_KFIFO(name, size);
                INIT_KFIFO(name);

            3 推入队列数据：
                unsigned int kfifo_in(struct kfifo *fifo, const void *from, unsgined int len);
                将from指针所指的len字节数据拷贝到fifo所指的队列中，如果成功，则放回推入数据的
                字节大小，如果队列中的空闲字节小于len，则该函数值最多可拷贝队列可用空间那么多
                的数据，这样的话，返回值可能小于len，甚至会返回0，这意味着没有任何数据被推入。

            4 摘取队列数据
                unsigned int kfifo_out(struct kfifo *fifo, void *to, unsigned int len);
                从fifo所指的队列中拷贝出长度为len字节的数据到to所指的缓冲区中，如果成功，该函数
                则返回拷贝的数据长度，如果队列中数据大小小于len，则该函数拷贝出的数据必然小于需要的
                数据大小。
                当数据被摘取后，数据不再存在于队列之中。

                unsigned int kfifo_out_peek(struct kfifo *fifo, void *to, unsigned int len, unsigned offset);
                    摘取数据而不删除，offset队列中的索引位置，如果为0，则读队列头。

            5 获取队列长度
                static inline unsigned int kfifo_size(struct kfifo *fifo);
                获取用于存储kfifo队列的空间的总体大小。

                static inline unsigned int kfifo_len(struct kfifo *fifo);
                返回kfifo队列中已推入数据的大小

                static inline unsigned int kfifo_avail(struct kfifo *fifo);
                获取队列中的可用空间

                static inline int kfifo_is_empty(struct kfifo *fifo);
                static inline int kfifo_is_full(struct kfifo *fifo);

            6 重置和撤销队列

                重置kfifo，抛弃队列中的内容
                static inline void kfifo_reset(struct kfifo *fifo);

                撤销一个使用kfifo_alloc()分配的队列
                void kfifo_free(struct kfifo *fifo);

            7 example:
#endif
static void exkfifo(struct kfifo *fifo)
{
    unsigned int i;
    unsigned int val;
    int ret;
    for(i = 0; i < 32; i++)
        kfifo_in(fifo, &i, sizeof(i));
    ret = kfifo_out_peek(fifo, &val, sizeof(val), 0);
    if(ret != sizeof(val))
        return -EINVAL;
    printk(KERN_INFO "%u\n", val);

    while(kfifo_avail(fifo)) {
        unsigned int val;
        int ret;

        ret = kfifo_out(fifo, &val, sizeof(val));
        if(ret != sizeof(val))
            return -EINVAL;

        printk(KERN_INFO "%u\n", val);
    }
}
