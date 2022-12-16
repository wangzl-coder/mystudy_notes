#if 0
    1 重入：一个函数可以同时被几个线程调用，则称此线程是安全的，
    注：一个线程安全的函数并不能说明该函数是异步信号安全的
    2 以线程安全的方式管理FILE对象(递归锁)
    
        int ftrylockfile(FILE *fp);

        void flockfile(FILE *fp);
        void funlockfile(FILE *fp);
    
    不加锁的字符标准I/O例程：
        int getchar_unlocked(void);
        int getc_unlocked(FILE *fp);

        int putchar_unlocked(int c);
        int putc_unlocked(int c, FILE *fp);
        注：除非被flockfile(ftrylockfile)或funlockfile包围，否则尽量不要调用这四个函数，因为其会
        导致不可预期的结果（多线程的非同步访问等）


#endif
