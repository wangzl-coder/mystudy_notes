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
    
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *env_init[] = {"USER=wangzl", "PATH=/tmp", NULL};

#define perr_exit(fmt, arg...)                                          \
{                                                                       \
    fprintf(stderr, "[%s]<%d> :"fmt, __FUNCTION__, __LINE__, ##arg);    \
    exit(-1);                                                           \
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
    demo_execle(); 
    exit(0);
}
