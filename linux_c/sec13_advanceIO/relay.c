#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define TTY1 "/dev/tty1"
#define TTY2 "/dev/tty2"

#define BUFSIZE (1024)

enum fsm_state{
    STATE_R = 0x0,
    STATE_W,
    STATE_ERR,
    STATE_T
};

struct fsm_t{
    int fds;
    int fdd;
    int state;
    int len;
    char *strerr;
    char *pos;
    char buff[BUFSIZE];
};

static int fsm_driver(struct fsm_t *fsm)
{
    int wlen;
    switch(fsm->state) {
        case STATE_R:
            fsm->len = read(fsm->fds, fsm->buff, BUFSIZE);
            if(fsm->len == 0)
                fsm->state = STATE_T;
            else if(fsm->len < 0) {
                if(errno != EAGAIN) {
                    fsm->strerr = "read error \r\n";
                    fsm->state = STATE_ERR;
                }
            } else {
                fsm->pos = fsm->buff;
                fsm->state = STATE_W;
            }
            break;
        case STATE_W:
            wlen = write(fsm->fdd, fsm->pos, fsm->len);
            if(wlen < 0) {
                if(errno != EAGAIN)
                    fsm->strerr = "write error \r\n";
                    fsm->state = STATE_ERR;
            } else {
                if(wlen < fsm->len) {
                    fsm->pos += wlen;
                    fsm->len -= wlen;
                } else
                    fsm->state = STATE_R;
            }
            break;
        case STATE_ERR:
            perror(fsm->strerr);
            fsm->state = STATE_T;
            break;
    
        case STATE_T:
            /* do something */
            break;

        default:
            abort();
            break;
    }
    return 0;
}

int relay(int fd1, int fd2)
{
    int flag1, flag2;
    struct fsm_t fsm12, fsm21;

    flag1 = fcntl(fd1, F_GETFL);
    flag2 = fcntl(fd2, F_GETFL);
    fcntl(fd1, F_SETFL, flag1 | O_NONBLOCK);
    fcntl(fd2, F_SETFL, flag2 | O_NONBLOCK);
    
    fsm12.fds = fd1;
    fsm12.fdd = fd2;
    fsm12.state = STATE_R;
    
    fsm21.fds = fd2;
    fsm21.fdd = fd1;
    fsm21.state = STATE_R;
    while(fsm12.state != STATE_T || fsm21.state != STATE_T) {
        fsm_driver(&fsm12);
        fsm_driver(&fsm21);
    }

    fcntl(fd1, F_SETFL, flag1);
    fcntl(fd2, F_SETFL, flag2);
    return 0;
}

int main(void)
{
    int fd1, fd2;
    int ret = 0;

    fd1 = open(TTY1, O_RDWR);
    if(fd1 < 0) {
        fprintf(stderr, "open %s failed : %s \r\n", TTY1, strerror(errno));
        exit(-1);
    }
    fd2 = open(TTY2, O_RDWR | O_NONBLOCK);
    if(fd2 < 0) {
        fprintf(stderr, "open %s failed : %s \r\n", TTY2, strerror(errno));
        ret = -1;
        goto close_fd1;
    }
    
    relay(fd1, fd2);
    close(fd2);
close_fd1:
    close(fd1);
    exit(ret);
}
