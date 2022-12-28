#if 0
    1 io多路转接：
        使用场景：从两个描述符阻塞读，由于第一个读的阻塞导致第二个描述符的读无法被处理(例如telnet)

        常见处理方式：
            a. 多进程：每个进程处理一条数据通路，缺点：程序设计复杂
            b. 多线程: 缺点: 程序设计复杂
            c. 采用非阻塞轮询读取，缺点：浪费CPU资源
            d. 异步IO，当描述符准备好可以进行IO时，内核用一个信号通知。缺点：可移植性；信号的单一性。

        I/O多路转接：构造一张感兴趣的描述符列表，调用一个函数，直到这些描述符中的一个已准备好进行IO时，该函数返回。
        poll,pselect和select这三个函数是我们能够执行IO多路转接。

    2 函数select 和 pselect
        int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execptfds, struct timeval *timeout);
        @timeout: 愿意等待的时间长度
        @readfds: 关心的可读的描述符集
        @writefds: 关心的可写的描述符集
        @execptfds: 关心的处于异常条件的描述符集
        @nfds: 最大文件描述符编号值加1

        @return: a. 返回-1表示出错，例如捕捉到一个信号。
                 b. 返回0表示没有描述符准备好（时间已过）
                 c. 返回正值说明已准备好的描述符数，该值是三个描述符集中已准备好的描述符数之和。在这种情况下
                 三个描述符集中仍旧打开的位对应于已准备好的描述符。
        准备好的含义：
        a. 对于读集(readfds)中的一个描述符进行的read操作不会阻塞，则认为此描述符是准备好的。
        b. 对于写集(writefds)中的一个描述符进行的write操作不会阻塞，则认为此描述符是准备好的。
        c. 对异常条件集(execptfds)中的一个描述符有一个未决异常条件，则认为此描述符是准备好的。异常条件包括：在网络
        连接上到达带外的数据，或者在处于数据包模式的伪终端上发生了某些条件。
        d. 对于读，写和异常条件，普通文件的文件描述符总是返回准备好。
        注：在一个描述符碰到了文件尾端，则select会认为该描述符是可读的，调用read会返回0

        设置描述符集位：
        int FD_ISSET(int fd, fd_set *fdset);
        void FD_CLR(int fd, fd_set *fdset);
        void FD_SET(int fd, fd_set *fdset);
        void FD_ZERO(fd_set *fdset);
  
        pselect:
        
        int pselect(int nfds, fd_set *readfds, fd_set *writefds, 
                    fd_set *execptfds, const struct timespec *timeout, const sigset_t *sigmask);
        与select差异：
            a. 超时值使用timespec描述。
            b. 超时值被声明为const， 保证调用pselect不会改变此值。
            c. pselect 可使用可选信号屏蔽字，若sigmask为NULL，则在与信号有关的方面，pselect与select相同。否则，在调用
            pselect时，以原子操作的方式安装该信号屏蔽字，在返回时，恢复以前的信号屏蔽字。


        3 函数poll
            int poll(struct pollfd *fds, nfds_t nfds, int timeout)
            与select不同，poll不是为每个条件（可读性，可写性和异常条件）构造的一个描述符集，而是构造一个pollfd结构的数组，
            每个数组元素指定一个描述符编号以及我们对该描述符感兴趣的条件
            struct pollfd {
                int fd; /* file descriptor */
                short events;   /* requested events */
                short revents;  /* returned events, set by kernel */
            }
            events:
                a. POLLIN: 可以不阻塞的读高优先级数据以外的数据（等效于POLLRDNORM | POLLRDBAND）
                b. POLLRDNORM : 可以不阻塞的读普通数据
                c. POLLRDBAND: 可以不阻塞的读优先级数据
                d. POLLPRI: 可以不阻塞地读高优先级数据
                e. POLLOUT : 可以不阻塞的写普通数据
                f. POLLWRNORM: 与POLLOUT相同
                g. POLLWRBAND: 可以不阻塞地写优先级数据
                h. POLLERR: 已出错
                i. POLLHUP: 已挂断
                j. POLLINVAL: 描述符没有引用一个打开的文件
#endif
