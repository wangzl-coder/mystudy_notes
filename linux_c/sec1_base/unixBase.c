#if 0
1: Unix 体系结构
    a.操作系统：控制硬件资源，提供程序运行环境的软件

    b.结构：内核<----- 系  统  调  用 <------库函数
                     /\           /\        /\
                     ||           ||        ||
                     ||           ||        ||
                     ||           ||        ||
                    shell<------- application


2: 登录
    a.登录名：/etc/passwd
    b.shell：

3：文件和目录(Ex:demo_ls)
    a.文件系统：目录项，文件属性
    b.文件名：不能出现/和空操作符。.（当前目录）和..（父目录）。
    c.路径名：绝对路径，相对路径
    d.工作目录（当前工作目录）
    e.起始目录（家目录）

4：输入输出()
    a.文件描述符：非负整数，标识特定进程正在访问的文件
    b.标准输入（standard input），标准输出(standard output)，标准出错(standard error)，重定向。
    c.不用缓冲的I/O：open,read,write,lseek,close(Ex:demo_content_copy)
    d.标准I/O：带缓冲，(Ex：demo_stdio_copy)

5：程序和进程
    a.程序：存放磁盘某个目录的可执行文件，kernel使用exec函数族将其加载到内存并执行.
    b.进程和进程ID：程序执行的实例称进程，唯一标识符进程ID（非负整数）（Ex:demo_getpid）
    c.进程控制：fork，exec，waitpid（demo_shell）
    d.线程和线程ID：进程内线程共享同一地址空间，文件描述符，栈，相关进程属性。

6：出错处理(Ex:demo_errno)
    a.errno：无错误时不清零，出错时才校验，errno没有置0操作。多线程存取errno。
    b.strerror(int errnum), perror(const char *msg), 
    c.出错恢复：致命出错和非致命出错

7：用户标识(Ex:demo_ugid())
    a.用户ID：标识不同用户，权限处理，root用户ID为0，映射用户名
    b.组ID：成员之间的资源共享，/etc/group,映射组名
    c.附加组ID：

8：信号（Ex:demo_signal）
    a.异步和同步
    b.进程信号处理选择：忽略，系统默认处理，捕捉。

9：时间值
    a.日历时间：time_t,自1970年1月1日00:00:00以来国际标准时间（格林尼治时间UTC）所经过的秒数，记录文件最近一次修改等。
    b.进程时间：也称CPU时间，度量进程使用CPU资源，以时钟滴答计算，clock_t。
        三个进程时间值：
        时钟时间（墙上时钟时间）：进程运行的时间总量
        用户CPU时间：应用层面
        系统CPU时间：内核层面
    c.time命令

10：系统调用和库函数


#endif

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static int demo_ls(char *path)
{
    DIR *dp;
    struct dirent *dirp;
    if(path == NULL)
        return -1;
    
    dp = opendir(path);
    if(dp == NULL) {
        fprintf(stderr, "open directory %s failed",path);
        perror(NULL);
        return -1;
    }
    while((dirp = readdir(dp)) != NULL)
        printf("%s\n", dirp->d_name);
    
    closedir(dp);
    return 0;
}
/*
 * 考虑：
 *      1 重定向< >实现文件拷贝
 *      2 BUFFSIZE值影响的效率
 * */
#define BUFFSIZE 4096
static void demo_content_copy(void)
{
    int len;
    char buf[BUFFSIZE];

    while((len = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
        if(write(STDOUT_FILENO, buf, len) != len)
           fprintf(stderr, "write failed \r\n");
    if(len < 0)
        fprintf(stderr, "read failed\r\n");
}

/*
 * 单字符读取，到文件结尾getc返回EOF
 * */
static void demo_stdio_copy(void)
{
    int ch;
    while((ch = getc(stdin)) != EOF)
        if(putc(ch, stdout) == EOF)
            fprintf(stderr, "write failed \r\n");
    if(ferror(stdin))
        fprintf(stderr, "read failed \r\n");
}

static void demo_getpid(void)
{
    printf("hello world from process %d \r\n", getpid());
}

#define LINEMAX 50
static void demo_shell(void)
{
    char buf[LINEMAX];
    pid_t pid;
    int status;

    printf("wanglz%%");
    while(fgets(buf, LINEMAX, stdin) != NULL) {
        if(buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0';
        if(strlen(buf) == 0) {
            printf("wanglz%%");
            continue;
        }
        pid = fork();
        if(pid < 0) {
            fprintf(stderr, "fork failed \r\n");
        } else if(pid == 0){        /* child process */
            execlp(buf,buf,(char*)0);
            fprintf(stderr, "exec failed \r\n");
            exit(-1);
        }
        /* parent process */
        if(waitpid(pid, &status, 0) < 0)
            fprintf(stderr, "wait process %d failed \r\n", pid);
        printf("wanglz%%");
    }
}

static void demo_errno(void)
{
    fprintf(stderr, "EACCES: %s \r\n", strerror(EACCES));
    errno = ENOENT;
    perror("ENOENT");
}

static demo_ugid(void)
{
    printf("user id %d, group id %d \r\n", getuid(), getgid());
}

static void sig_int(int signum)
{
    printf("signal interrupt \r\n");
    exit(0);
}

/*
 * kill -SIGINT pid
 * */
static void demo_signal(void)
{
    if(signal(SIGINT, sig_int) == SIG_ERR)
        fprintf(stderr, "signal SIGINT failed \r\n");
    while(1);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    /*
    if(argc != 2) {
        fprintf(stderr, "invaild input param \r\n");
        return -1;
    }
    ret = demo_ls(argv[1]);
    */
//    demo_content_copy();
//   demo_stdio_copy();
//    demo_getpid();
//    demo_shell();
//    demo_errno();
//    demo_ugid();
    demo_signal();
    exit(ret);
}
