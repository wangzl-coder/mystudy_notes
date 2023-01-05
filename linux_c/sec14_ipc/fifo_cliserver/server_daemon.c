#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "server_daemon.h"

#define SBUFMAX PIPE_BUF
#define SERVER_PID  "/var/fifo_daemon.pid"
#define SERVER_PID_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define lock_file(fd)   ({      \
    struct flock fl;            \
    fl.l_type = F_WRLCK;        \
    fl.l_start = 0;             \
    fl.l_whence = SEEK_SET;     \
    fl.l_len = 0;               \
    fcntl(fd, F_SETLK, &fl);    \
})

struct server_t {
    int sfd;
    int curr_client;
    int rlen;
    char sbuf[SBUFMAX];
};

static struct server_t *server;

static void server_exit()
{
    if(server == NULL)
        return;
    close(server->sfd);
    free(server);
    remove(FIFO_SERVER);
}

static void server_start()
{
    int rlen;
    while((rlen = read(server->sfd, server->sbuf, SBUFMAX)) != 0) {
        if(rlen < 0) {
            if(errno == EINTR)
                continue;
            syslog(LOG_ERR, "fifo daemon read error \r\n");
            exit(-1);
        }
        syslog(LOG_INFO, "%s", server->sbuf);
    }
}

static int fifo_daemon_already()
{
    int fd;
    char buf[10];
    
    if((fd = open(SERVER_PID, O_RDWR | O_TRUNC | O_CREAT, SERVER_PID_MODE)) < 0) {
        syslog(LOG_ERR, "open %s failed : %s \r\n", SERVER_PID, strerror(errno));
        exit(-1);
    }
    if(lock_file(fd) < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "lock file error \r\n");
        exit(1);
    }
    snprintf(buf, sizeof(buf), "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return(0);
}

static void server_envinit(void)
{
    int i;
    int fd0, fd1, fd2;
    struct rlimit rlim;
    pid_t pid;
    struct sigaction act;

    umask(0);   
    
    if(getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        perror("cannot get limit \r\n");
        exit(-1);
    }
    if((pid = fork()) < 0) {
        perror("fork error \r\n");
        exit(-1);
    } else if(pid != 0)
        exit(0);
    setsid();

    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGHUP, &act, NULL) < 0) {
        perror("sigaction SIGHUP error \r\n");
        exit(-1);
    }

    if((pid = fork()) < 0) {
        perror("fork failed \r\n");
        exit(-1);
    } else if(pid != 0) {
        exit(0);
    }
    if(chdir("/") < 0) {
        perror("chdir to root failed \r\n");
        exit(-1);
    }
    if(rlim.rlim_max == RLIM_INFINITY)
        rlim.rlim_max = 1024;
    for(i = 0; i < rlim.rlim_max; i++)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    openlog("fifo_server", LOG_CONS, LOG_DAEMON);
    syslog(LOG_ERR, "open log \r\n");
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d\n", fd0, fd1, fd2);
        exit(-1);
    }
}

int main(void)
{
    server_envinit();
    if(fifo_daemon_already()) {
        syslog(LOG_ERR, "daemon is already running \r\n");
        exit(-1);
    }
    if(server == NULL) {
        server = malloc(sizeof(struct server_t));
        if(server == NULL) {
            syslog(LOG_ERR, "server malloc error : %s \r\n", strerror(ENOMEM));
            exit(ENOMEM);
        }
    }
    if(mkfifo(FIFO_SERVER, 0600) < 0) {
        syslog(LOG_ERR, "mkfifo %s error : %s \r\n", FIFO_SERVER, strerror(errno));
        exit(-1);
    }
    //atexit();
    server->sfd = open(FIFO_SERVER, O_RDWR | O_CREAT);   /* open by read write to avoid recv end of file */
    server_start();
    exit(0);
}
