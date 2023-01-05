#if 0
    FIFO:
        1 有名管道：不相关的进程间数据的交换（文件类型）
    
        2 
            int mkfifo(const char *pathname, mode_t mode);
            int mkfifoat(int dirfd, const char *pathname, mode_t mode);
            @mode: 与open中的mode一样
            两个函数差异：
            （1）pathname指定绝对路径，fd会被忽略，并且mkfifoat和mkfifo行为一致
            （2）pathname指定相对路径，fd是一个打开目录的有效文件描述符，路径名和目录有关
            （3）pathname指定相对路径，并且fd有一个特殊值AT_FDCWD,则路径名以当前目录开始，mkfifo和mkfifoat类似。

        创建FIFO后，需要open打开
        非阻塞打开（O_NONBLOCK）会产生以下影响：
        （1）未指定O_NONBLOCK时，只读open要阻塞到其他进程写打开FIFO。只写打开也阻塞直到其他进程读打开FIFO
        （2）如果指定O_NONBLOCK,只读open立即返回。如果没有进程读打开FIFO，则写打开将返回-1，并将errno设置成ENXIO

        FIFO用途：
        （1）shell 命令使用FIFO将数据从一条管道传送到另一条时，无需创建中间临时文件
        （2）客户进程-服务器进程应用程序中，FIFO用作汇聚点，在客户进程和服务进程二者之间传递数据
    
        实例：
        （1）复制输出流：
            mkfifo fifo1
            prog3 < fifo1 &
            prog1 < infile | tee fifo1 | prog2

        （2）客户进程-服务进程通信

#endif
