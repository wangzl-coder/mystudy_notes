#if 0
    进程间通信--管道
    1 局限性：
        （1）半双工
        （2）只能在具有公共祖先的两个进程之间使用。

    2 创建：
        int pipe(int pipefd[2]);
        @return : 0 for sunccess, or -1 for error
        回填两个文件描述符，pipefd[0]为读而打开，pipefd[1]为写而打开
        fstat函数对管道的每一端都返回一个FIFO类型的文件描述符。

        （1）当读一个写端已被关闭的管道时，在所有数据都被读取后，read返回0，表示文件结束。
        （2）如果写一个读端被关闭的管道，则产生信号SIGPIPE。如果忽略该信号或者捕捉该信号并
        从其处理程序返回，则write返回-1，errno设置为EPIPE；
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define MAXLINE (50)
#if 0
        /* 管道简单使用 */
int main()
{
    pid_t pid;
    int fd[2];

    if(pipe(fd) < 0) {
        fprintf(stderr, "failed to pipe \r\n");
        exit(-1);
    }
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        close(fd[0]);
        close(fd[1]);
        exit(-1);
    } else if(pid == 0) {
        int n;
        char line[MAXLINE];
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
        close(fd[0]);
    } else {
        close(fd[0]);
        write(fd[1], "hello world", 12);
        close(fd[1]);
        waitpid(pid);
    }
    exit(0);   
}
#endif

/* 分页输出：管道读端重定向到标准输入，调用分页程序 */
#define DEF_PAGER  "/bin/more"
int main(int argc, char *argv[])
{
    pid_t pid;
    int fd[2];
    FILE *fp;

    if(argc != 2) {
        fprintf(stderr, "usage: pipe <pathname>");
        exit(-1);
    }
    if((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "fopen %s failed \r\n", argv[1]);
        exit(-1);
    }

    if(pipe(fd) < 0) {
        fprintf(stderr, "pipe failed \r\n");
        exit(-1);
    }
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        close(fd[0]);
        close(fd[1]);
        exit(-1);
    } else if(pid == 0) {
        char *pager;
        char *argv0;
        close(fd[1]);
        if(fd[0] != STDIN_FILENO) {
            int ret = 0;
            if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
                fprintf(stderr, "dup failed \r\n");
                ret = -1;
            }
            close(fd[0]);   /* don't need this after dup2 */
            if(ret)
                exit(ret);
        }
        /* get arguments for execl */
        if((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        if((argv0 = strrchr(pager, '/')) != NULL)
            argv0 ++;
        else 
            argv0 = pager;
        
        printf("argv0 %s \r\n", argv0);
        if(execl(pager, argv0, (char *)0) < 0) {
            fprintf(stderr, "execl failed \r\n");
            exit(-1);
        }
    } else {
        char line[MAXLINE];
        int n;

        close(fd[0]);   
        while(fgets(line, MAXLINE, fp) != NULL) {
            n = strlen(line);
            if(write(fd[1], line, n) != n) {
                fprintf(stderr, "error to write to pipe \r\n");
                exit(-1);
            }
        }
        if(ferror(fp)) {
            fprintf(stderr, "fgets error \r\n");
        }
        close(fd[1]);
        if(waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "waitpid error \r\n");
        }
        exit(0);
    }
}


/* 父子进程管道的通知法实现（信号实现请查阅前面的signal章节） */
void TELL_WAIT(int fdp[2], int fdc[2])
{
    if(pipe(fdp) < 0 || pipe(fdc) < 0)
        fprintf(stderr, "pipe failed \r\n");
}

void TELL_PARENT(int fdp[2])
{
    close(fdp[0]);
    if(write(fdp[1], "p", 1) != 1) {
        fprintf(stderr, "write to fdp failed \r\n");
    }
}

void WAIT_PARENT(int fdp[2])
{
    close(fdp[1]);
    char c;
/* 忽略信号打断 */
    if(read(fdp[0], &c, 1) != 1){
        fprintf(stderr, "read error \r\n");
        return;
    }
    if(c != 'p') {
        fprintf(stderr, "incorrect parent data \r\n");
    }
}

void TELL_CHILD(int fdc[2])
{
    close(fdc[0]);
    if(write(fdc[1], "c", 1) != 1) {
        fprintf(stderr, "write to child error \r\n");
    }
}

void WAIT_CHILD(int fdc[2])
{
    close(fdc[1]);
    char c;
    if(read(fdc[0], &c, 1) != 1) {
        fprintf(stderr, "read child error \r\n");
        return;
    }
    if(c != 'c') {
        fprintf(stderr, "incnrrect child data \r\n");
    }
}
