#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define POS 10
#define BUFFSIZE POS
#define BURST 100

static int token = 0;

static void sigalrm_handler(int signum)
{
    if(++token > BURST)
        token = BURST;
    alarm(1);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int fd;
    int len;
    char buf[BUFFSIZE];
    char *ptr;


    if(argc != 2) {
        fprintf(stderr, "invaild param \r\n");
        exit(-1);
    }
    if((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open failed \r\n");
        exit(-1);
    }
    signal(SIGALRM, sigalrm_handler);
    alarm(1);
    while(1) {

        while(token <= 0)
            pause();
        token --;
        while((len = read(fd, buf, BUFFSIZE)) < 0) {
            
            /* interrupt by signal, retry */
            if(errno == EINTR)
                continue;
            perror("read failed \r\n");
            ret = -1;
            break;
        }
        /* Re-judge */
        if(len <= 0)
            goto close_exit;
        ptr = buf;
        while(len > 0) {
            int wlen = write(STDOUT_FILENO, ptr, len);
            if(wlen < 0) {
                if(errno == EINTR)
                    continue;
                else {
                    perror("write failed ");
                    ret = -1;
                    goto close_exit;
                }
            }
            len -= wlen;
            ptr += wlen;
        }
    }
    
close_exit:
    close(fd);
    exit(ret);
}
