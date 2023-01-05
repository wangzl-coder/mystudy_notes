#if 0
    1 三种XSI的IPC：消息队列，信号量，共享存储

    2 相似点：
        （1）标识符和键
            a. 标识符：内核维护引用，非负整数，循环累加过程
            
            b. 键：IPC对象外部关联名，key_t,由内核变换成标识符
                创建一个键：
                key_t ftok(const char *pathname, int proj_id);
                @pathname: 现有的文件路径
                @proj_id: 0-255, 只使用低八位，
#endif
