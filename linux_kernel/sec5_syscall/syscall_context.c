#if 0
    系统调用上下文
        （1）处于进程上下文，current指针指向当前任务
        （2）在进程上下文中，内核可以休眠并且被抢占。
        （3）保证系统调用的可重入性。

        1 绑定系统调用的最后步骤：
            （1）在系统调用表的最后加入一个表项（entry.s），
            （2）对所支持的各种体系结构，系统调用号都必须定义与<asm/unistd.h>中
            （3）系统调用必须被编译进内核映像，不能被编译成模块。
#endif
            /* entry.s */
ENTRY(sys_call_table)
    .long sys_restart_syscall      /* 0 */
    .long sys_exit
    .long sys_fork
    .long sys_read
    .long sys_write
    .long sys_open                  /* 5 */

    ...

    .long sys_eventfd2
    .long sys_epoll_create1
    .long sys_dup3                  /* 330 */
    .long sys_pipe2
    .long sys_inotify_init1
    .long sys_preadv
    .long sys_pwritev
    .long sys_rt_tgsigqueueinfo     /* 335 */
    .long sys_perf_event_open
    .long sys_recvmmsg
    .long sys_foo                   /* 338 our new sys_call */



        /* 加入系统调用号，<asm/unistd.h> */
#define __NR_restart_syscall        0
#define __NR_exit                   1
#define __NR_fork                   2
    ...

#define __NR_foo                    338
        
        /* 实现foo()系统调用，编译进内核，放到相关文件 */
#include <asm/page.h>
/*
 * sys_foo - 返回每个进程的内核栈大小
 * */
asmlinkage long sys_foo(void)
{
    return THREAD_SIZE;
}

#if 0
    2 从用户空间访问系统调用
        （1）通常系统调用靠C库支持
        （2）对于新加的系统调用，Linux提供一组宏，用于直接对系统调用进行访问，其会设置好寄存器并调用陷入内核
            _syscalln(),n范围0-6，表示传参个数
#endif
    /* open 函数的系统调用 */
long open(const char *filename, int flags, int mode)
            ||
            ||
            \/
#define NR_OPEN 5
_syscall3(long, open, const char *, filename, int, flags, int, mode)
/* 
 * 第一个参数：返回值类型
 * 第二个参数：系统调用名称
 * */


    /* foo()的系统调用 */
#define __NR_doo 283
_syscall0(long, foo)

int main()
{
    long stack_size;
    stack_size = foo();
    printf("The kernel stack size is %ld\n", stack_size);
    return 0;
}


#if 0
    为什么不通过系统调用的方式实现:
    （1）需要一个系统调用号，需要在内核处于开发版本的时候由官方分配
    （2）系统调用被加入稳定内核就被固化，为避免应用程序的崩溃，其接口不允许做改动
    （3）需要将系统调用分别注册到每个需要支持的体系结构中去
    （4）在脚本中不容易调用系统调用，也不能从文件系统直接访问系统调用
    （5）由于需要系统调用号，因此在主内核树之外是很难维护和使用的
    （6）如果只进行简单的信息交换，系统调用有些大材小用
    代替方法：
        实现一个设备节点，并对此实现read，write。使用ioctl对特定的设置进行操作或者对特定的信息进行检索
#endif
