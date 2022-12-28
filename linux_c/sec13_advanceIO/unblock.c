#if 0
    1 系统调用分成两类：“低速”系统调用和其他，低速系统调用是可能会使进程永远阻塞的一类系统调用，包括：
        （1）如果某些文件类型（如读管道，终端设备和网络设备）的数据并不存在，读操作可能会使调用者永远阻塞。
        （2）如果数据不能被相同的文件类型立即接受（如管道中无空间，网络流控制），写操作可能会使调用者永远阻塞。
        （3）再某种条件发生之前打开某些文件类型可能会发生阻塞（如要打开一个终端设备，需要先等待与之连接的调制解调器
            应答，又如以只写模式打开FIFO，那么在没有其他进程已用读模式打开该FIFO时也要等待。
        （4）对已经加上强制性记录锁的文件进行读写。
        （5）某些ioctl操作
        （6）某些进程间通信函数
        虽然读写磁盘文件会暂时阻塞调用者，但不能将与磁盘IO有关的系统调用视为低速
    
    2 非阻塞IO：非阻塞IO使我们可以发出后open,read,和write这样的IO操作，并使这些操作不会永远阻塞，如果这种操作不能
        完成，则调用立即出错返回，表示该操作如继续执行将阻塞。
        （1）如果调用open获得描述符，则可指定O_NONBLOCK标志
        （2）对于已经打开的一个描述符，则可调用fcntl，由该函数打开O_NONBLOCK文件状态标志，
#endif
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main()
{
    char buf[500000];
    int rdlen, wrlen = 0;
    char *ptr;

    rdlen = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes \r\n", rdlen);

    /* set nonblock for stdout */
    fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);
    ptr = buf;
    while(rdlen > 0) {
        ptr += wrlen;
        wrlen = write(STDOUT_FILENO, ptr, rdlen);
        fprintf(stderr, "write %d bytes, errno = %d \r\n", wrlen, errno);
        rdlen -= wrlen;
    }
    exit(0);
}
