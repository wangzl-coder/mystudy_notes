#if 0
    1 函数readv和writev：readv和writev用于在一次函数调用中读写多个非连续缓冲区，有时也将这两个函数称为
    散布读（scatter read）和聚集写（gather write）
    
    ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
    ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
    struct iovec {
        void *iov_base;     /* starting address of buffer */
        size_t iov_len;     /* size of buffer */
    };
    @return 已读或已写的字节总数，若出错，返回-1

    2 函数readn和writen
        （1）管道，FIFO,以及某些设备（特别是终端和网络）有下列两种性质：
            a. 一次read操作所返回的数据可能少于要求的数据，即使还没达到文件尾端也可能是这样，这不是一个
            错误，应该继续读该设备
            b. 一次write操作的返回值也可能少于指定输出的字节数，这可能由某个因素造成的，例如，内核输出缓冲区
            变满，这也不是错误，应当继续写余下数据（通常，只有非阻塞描述符，或捕捉到一个信号时，才发生这种
            write的中途返回）
            在读写磁盘文件时从未见到这种情况，除非文件系统用完了空间，或者接近了配额限制，不能将要求写二点数据全部写出
            通常，在读写一个管道，网络设备或终端时，需要考虑这些特性，下面两个函数readn和writen的功能分别是读写指定的N
            字节数据，并处理返回值可能小于要求值的情况。这两个函数只是按需多次调用read和write直至读写了N字节数据

        (2)
            ssize_t readn(int fd, void *buf, size_t nbytes)
            ssize_t writen(int fd, void *buf, size_t nbytes);

#endif
/* readn 和 writen 的实现 */
ssize_t readn(int fd, void *buf, size_t nbytes)
{
    size_t nleft;
    size_t nread;

    nleft = nbytes;
    while(nleft > 0) {
        if((nread = read(fd, buf, nleft)) < 0) {
            if(nleft == nbytes)
                return (-1);
            else 
                break;
        } else if(nread == 0) {
            break;
        }
        nleft -= nread;
        buf += nread;
    }
    return (nbytes - nleft);
}

size_t writen(int fd, void *buf, size_t nbytes)
{
    size_t nwrite;
    size_t nleft;
    
    nleft = nbytes;
    while(nleft > 0) {
        if((nwrite = write(fd, buf, nleft)) < 0) {
            if(nleft == nbytes)
                return (-1);
            else 
                break;
        } else if(nwrite == 0) {
            break;
        }
        nleft -= nwrite;
        buf += nwrite;
    }
    return (nbytes - nleft)
}
