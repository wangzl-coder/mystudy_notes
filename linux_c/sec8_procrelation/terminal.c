#if 0
    1 终端登录
        a. 点对点
        b. 终端登录：getty（0, 1, 2设备描述符连接）->login->fork+exec->shell, /etc/inittab,
    2 网络登录：
        a. 非点对点，伪终端
        b. 过程：init调用shell执行/etc/rcc,启动守护进程inetd，等待TCP连接请求（telnet hostname），登录到服务进程所在主机，
            telnetd打开伪终端设备并fork，父进程处理通过网络联机的通信，子进程执行login程序，父子进程通过伪终端相连，exec之前
            子进程使文件描述符0,1,2与伪终端相连，登录正确->更改当前目录为起始目录，设置登录用户的组ID与用户ID，以登录用户的
            初始环境，然后login调用exec将其自身替换为登录用户的登录shell
    3 进程组
        a. 一个或多个进程的集合，与同一作业相关联，可以接收来自同一终端的各种信号，可用pid_t getpgrp(void)获取进程组ID
        ...
    4 作业控制
        


#endif
