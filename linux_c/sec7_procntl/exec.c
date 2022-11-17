#if 0
1 exec 函数族
    a. 调用一种exec函数时，用全新的程序替换当前进程的正文，数据，堆和栈段。fork,exec,wait是基本的进程控制原语
    b. 
        int execl(const char *path, const char *arg, .../* (char *) NULL */);
        int execlp(const char *file, const char *arg, .../* (char *) NULL */);
        int execle(const char *path, const char *arg, ...
                /* (char *) NULL */, char *const envp[]);
        int execv(const char *path, char *const argv[]);
        int execvp(const char *file, char *const argv[]);
        int execvpe(const char *file, char *const argv[], char *const envp[]);
    l表示命令行参数列表，p表示PATH环境变量，v表示使用参数数组，e使用环境变量数组:

2 更改用户ID和组ID
    a. 最小特权模型
    b. 
        int setuid(uid_t uid);
        int setgid(gid_t gid);
    c. 改变用户ID规则
        （1）进程具有超级用户特权，setuid将实际用户ID，有效用户ID，保存到设置用户ID设置为uid
        （2）进程没有超级用户特权，uid等于实际用户ID或保存的设置用户ID，setuid只将有效用户ID设置为uid
        （3）都不满足，errno设置为EPERM，返回-1

3 解释器文件
    a. exec执行解释器文件（demo_exec_interpreter）   

4 system函数
    a. int system(const char *cmdstring);
        实现方式：fork + exec + waitpid
        @return:(1) cmdstring == NULL -> return +
                (2) fork失败或waitpid返回出EINTR之外的出错，system返回-1，并设置errno
                (3) exec失败(表示不能执行shell)，则其返回值同shell执行exit(127)一样
                (4) 均执行成功，返回shell的终止状态
    b. system实现（未做信号处理，demo_system_achive）
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

char *env_init[] = {"USER=wangzl", "PATH=/tmp", NULL};

#define perr_exit(fmt, arg...)                                          \
{                                                                       \
    fprintf(stderr, "[%s]<%d> :"fmt, __FUNCTION__, __LINE__, ##arg);    \
    exit(-1);                                                           \
}

static int demo_system_achive(const char *cmdstring)
{
    int status;
    pid_t pid;
    
    if(cmdstring == NULL)
        return(1);

    if((pid = fork()) < 0){
        status = -1;
    } else if(pid == 0) {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    } else {
        while(waitpid(pid, &status, 0) < 0) {
            if(errno != EINTR) {
                status = -1;
                break;
            }
        }
    }
    return status;
}

static void demo_exec_interpreter()
{
    pid_t pid;
    int errnum;
    if((pid = fork()) < 0)
        perr_exit("fork failed\r\n")
    else if(pid == 0) {
        errnum = execl("/home/wangzl/workSpace/mystudy_notes/linux_c/sec7_procntl/testinterp", "testinterp", "arg1", (char *)0);
        perr_exit("exec failed(%d) \r\n", errnum);
    }
    if(waitpid(pid, NULL, 0) != pid)
        fprintf(stderr, "wait child %d failed \r\n", pid);
}

static void func_printenv()
{
    extern char **environ;
    char **ptr;
    printf("----------------------------pid:%d---------------------------\r\n",getpid());
    for(ptr = environ; *ptr != 0; ptr ++) {
        printf("%s \r\n", *ptr);
    }
    printf("-------------------------------------------------------------\r\n",getpid());
}

static void demo_execl()
{
    pid_t pid;
    func_printenv();
    if((pid = fork()) < 0)
        perr_exit("fork failed \r\n")
    else if(pid == 0) {
        printf("child %d execl \r\n", getpid());
        //execl("/home/wangzl/workSpace/mystudy_notes/linux_c/sec7_procntl/echoall", "echoall", "arg1", "arg2", NULL);
        execl("./echoall", "echoall", "arg1", "arg2", NULL);
        perr_exit("execl failed \r\n")
    }
    if(waitpid(pid, NULL, 0) != pid) {
        fprintf(stderr, "wait %d failed \r\n", pid);
    }
}

static void demo_execlp()
{
    pid_t pid;
    func_printenv();
    if((pid = fork()) < 0)
        perr_exit("fork failed \r\n")
    else if(pid == 0) {
        printf("child %d execl \r\n", getpid());
        execlp("echoall", "echoall", "arg1", "arg2", NULL);
        perr_exit("execl failed \r\n")
    }
    if(waitpid(pid, NULL, 0) != pid) {
        fprintf(stderr, "wait %d failed \r\n", pid);
    }
}

static void demo_execle()
{
    pid_t pid;
    func_printenv();
    if((pid = fork()) < 0)
        perr_exit("fork failed \r\n")
    else if(pid == 0) {
        printf("child %d execl \r\n", getpid());
        execle("./echoall", "echoall", "arg1", "arg2", NULL, env_init);
        perr_exit("execl failed \r\n")
    }
    if(waitpid(pid, NULL, 0) != pid) {
        fprintf(stderr, "wait %d failed \r\n", pid);
    }
}

int main()
{
//    demo_execl();
//    demo_execlp();
//    demo_execle(); 
//    demo_exec_interpreter();
    demo_system_achive("date");
    exit(0);
}
