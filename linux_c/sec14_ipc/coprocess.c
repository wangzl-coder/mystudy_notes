#if 0
    协同进程：
        1 过滤程序
        2 协同进程：一个过滤程序既产生某个过滤程序的输入，又读取该过滤程序的输出。

#endif
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define MAXLINE 50
#if 0
/* add2 */
int main(void)
{
    int n, int1, int2;
    char line[MAXLINE];

    while((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
        line[n] = 0;
        if(sscanf(line, "%d%d", &int1, &int2) == 2) {
            sprintf(line, "%d\n", int1 + int2);
            n = strlen(line);
            if(write(STDOUT_FILENO, line, n) != n)
                fprintf(stderr, "write error");
        } else {
            if(write(STDOUT_FILENO, "invalid args\n", 13) != 13)
                fprintf(stderr, "write error");
        }
    }
    exit(0);
}
#endif
#include <signal.h>


static void sig_pipe(int signo);
int main(void)
{
    int fd1[2], fd2[2];
    pid_t pid;

    if(signal(SIGPIPE, sig_pipe) == SIG_ERR) {
        fprintf(stderr, "signal SIGPIPE err");
        exit(-1);
    }
    if(pipe(fd1) < 0 || pipe(fd2) < 0) {
        fprintf(stderr, "pipe error");
        exit(-1);
    }
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed \r\n");
        exit(-1);
    } else if(pid > 0) {
        int n;
        char line[MAXLINE];
        close(fd1[1]);
        close(fd2[0]);
        
        while(fgets(line, MAXLINE, stdin) != NULL) {
            n = strlen(line);
            if(write(fd2[1], line, n) != n)
                fprintf(stderr, "write error to pipe");
            if((n = read(fd1[0], line, MAXLINE)) < 0)
                fprintf(stderr, "read error from pipe");
            if(n == 0) {
                fprintf(stderr, "child closed pipe");
                break;
            }
            line[n] = 0;
            if(fputs(line, stdout) == EOF)
                fprintf(stderr, "fputs error");
        }
        if(ferror(stdin))
            fprintf(stderr, "fgets error on stdin");
        exit(0);
    } else {
        close(fd1[0]);
        close(fd2[1]);
        if(fd2[0] != STDIN_FILENO) {
            if(dup2(fd2[0], STDIN_FILENO) != STDIN_FILENO)
                fprintf(stderr, "dup2 error to stdin");
            close(fd2[0]);
        }

        if(fd1[1] != STDOUT_FILENO) {
            if(dup2(fd1[1], STDOUT_FILENO) != STDOUT_FILENO)
                fprintf(stderr, "dup2 error to stdout");
            close(fd1[1]);
        }
        if(execl("./add2", "add2", (char *)0) < 0)
            fprintf(stderr, "execl error");
    }
    exit(0);
}

static void sig_pipe(int signo)
{
    printf("SIGPIPE caught \r\n");
    exit(1);
}
