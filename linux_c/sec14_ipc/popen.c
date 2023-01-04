#if 0
    1 函数popen和pclose
        标准IO库，实现操作：创建一个管道，fork一个子进程，关闭未使用的管道端，执行一个shell运行命令，等待命令终止。
        FILE *popen(const char *cmdstring, const char *type);
        @return : 返回标准IO文件指针，type为"r"时，指向连接到cmdstring的标准输出。type为"w"时，指向cmdstring的标准输入
        
        int pclose(FILE *fp);
        关闭标准IO流，等待命令终止，返回shell终止状态。

#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <open_max.h>

/* 利用popen实现翻页查看 */

#if 0
#define PAGER "${PAGER:-more}"  /* enviroment variable , or default */
#define MAXLINE (100)

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    FILE *fpin, *fpout;
    int ret = 0;

    if(argc != 2) {
        fprintf(stderr, "usage: popen <pathname> \r\n");
        exit(-1);
    }
    if((fpin = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "cannot open %s \r\n", argv[1]);
        exit(-1);
    }
    if((fpout = popen(PAGER, "w")) == NULL) {
        ret = -1;
        goto close_fpin;
    }
    while(fgets(line, MAXLINE, fpin) != NULL) {
        if(fputs(line, fpout) == EOF) {
            fprintf(stderr, "fputs error pipe");
            ret = -1;
            break;
        }
    }
    if(ferror(fpin)) {
        ret = -1;
        fprintf(stderr, "error read %s \r\n", argv[1]);
    }

    if(pclose(fpout) == -1) {
        ret = -1;
        fprintf(stderr, "fgets error \r\n");
    }

close_fpin:
    close(fpin);
    exit(ret);
}
#endif

/* 
 * pointer to array allocated at run-time
 * */
static pid_t *childpid = NULL;


static int maxfd;

/* popen 和 pclose 实现 */
FILE *popen(const char *cmdstring, const char *type)
{
    int i;
    int pfd[2];
    pid_t pid;
    FILE *fp;

    /* only allow "w" or "r" */
    if((*type != 'r' && *type != 'w') || (type[1] != 0)) {
        errno = EINVAL;
        return NULL;
    }
    
    if(childpid == NULL) {        /* first time through */
        /* allocate zeroed out array for child pids */
        maxfd = open_max();
        if(childpid = calloc(maxfd, sizeof(pid_t)) == NULL)
            return NULL;
    }
    
    if(pipe(pfd) < 0)
        return NULL;    /* errno set by pipe() */
    if(pfd[0] >= maxfd || pfd[1] >= maxfd) {
        close(pfd[0]);
        close(pfd[1]);
        errno = EMFILE;
        return(NULL);
    }

    if((pid = fork()) < 0) {
        return NULL;
    } else if(pid == 0) {
        if(*type == 'r') {
            close(pfd[0]);
            if(pfd[1] != STDOUT_FILENO) {
                dup2(pfd[1], STDOUT_FILENO)
                close(pfd[1]);
            }
        } else {
            close(pfd[1]);
            if(pfd[0] != STDIN_FILENO) {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]);
            }
        }

        /* close all descriptors in childpid[] */
        for(i = 0; i < maxfd; i++)
            if(childpid[i] > 0)
                close(i);

        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }

    /* parent continues... */
    if(*type == 'r') {
        close(pfd[1]);
        if((fd == fdopen(pfd[0], type)) == NULL)
            return NULL;
    } else {
        close(pfd[0]);
        if((fd == fdopen(pfd[1], type)) == NULL)
            return NULL;
    }
    
    childpid[fileno(fp)] = pid;
    return fp;
}

int pclose(FILE *fp)
{
    int fd, stat;
    pid_t pid;

    if(childpid == NULL) {
        errno = EINVAL;
        return (-1);
    }
    fd = fileno(fp);
    if(fd >= maxfd) {
        errno = EINVAL;
        return (-1);
    }
    if((pid = childpid[fd]) == 0) {
        errno = EINVAL;
        return (-1);
    }
    
    childpid[fd] = 0;
    if(fclose(fp) == EOF)
        return -1;

    while(waitpid(pid, &stat, 0) < 0)
        if(errno != EINTR)
            return (-1);

    return (stat)
}

