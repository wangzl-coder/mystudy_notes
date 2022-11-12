#if 0
1 main函数
    a. C程序总是从main函数开始执行 int main(int argc, char *argv[]);
    b. 启动例程，连接编辑器

2 进程终止
    a. 进程终止方式：
        正常终止：
            (1) 从main return
            (2) 调用exit()
            (3) 调用_exit 或 _Exit
            (4) 最后一个线程从启动例程返回
            (5) 最后一个线程调用pthread_exit
        异常终止：
            (6) 调用abort
            (7) 接到一个信号并终止
            (8) 最后一个线程对取消请求做出响应

    b. exit
        void exit(int status);
        void _Exit(int status);
        void _exit(int status);
        注意：
            (1) exit会在退出前做终止处理程序标准IO的清理关闭等操作，会造成fflush
            (2) 终止状态：未定义终止状态，echo $?

    c. 钩子函数atexit() demo_atexit()
       int atexit(void (*func) (void)), 成功返回0
        逆序调用 
        作用：资源释放回收，状态恢复函数挂钩

    
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void func_exit1(void)
{
    puts(__FUNCTION__);
}

static void func_exit2(void)
{
    puts(__FUNCTION__);
}

int main(void)
{
    atexit(func_exit1); 
    atexit(func_exit2);
    atexit(func_exit2);
    exit(0);
//    _exit(0);
}
