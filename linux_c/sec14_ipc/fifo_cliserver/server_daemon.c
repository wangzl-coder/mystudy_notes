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


#define SERVER_PID  "/tmp/myfifo_daemon/fifo_daemon.pid"
#define SERVER_PID_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define lock_file(fd)   ({      \
    struct flock fl;            \
    fl.l_type = F_WRLCK;        \
    fl.l_start = 0;             \
    fl.l_whence = SEEK_SET;     \
    fl.l_len = 0;               \
    fcntl(fd, F_SETLK, &fl);    \
})

#define unlock_file(fd)   ({      \
    struct flock fl;            \
    fl.l_type = F_UNLCK;        \
    fl.l_start = 0;             \
    fl.l_whence = SEEK_SET;     \
    fl.l_len = 0;               \
    fcntl(fd, F_SETLK, &fl);    \
})

enum sstate_e{
    SSTATE_R = 0x0,
    SSTATE_PASER,
    SSTATE_INVALID_REQ,
    SSTATE_RESP,
    SSTATE_CONN,
    SSTATE_DISCON,
    SSTATE_ERR,
    SSTATE_STOP,
};

struct clisession_t{
    pid_t clipid;
    int clififd;
    char respmsg[RESPBUFSIZE];
};

struct server_t {
    int sfd;
    int state;
    int rlen;
    char sbuf[SBUFMAX];
    struct clisession_t curclient;
};

enum dir_action_e {
    ACTION_CREATE = 0x0,
    ACTION_REMOVE,
};

static struct server_t *server;

static void abspath_diraction(char *abspath, int action)
{
    char dir[200], *pos;

    pos = strrchr(abspath, '/');
    if(pos != NULL) {
        strncpy(dir, abspath, strlen(abspath) - strlen(pos));
        dir[strlen(abspath) - strlen(pos)] = 0;
    }
    if(action == ACTION_CREATE) {
        if(access(dir, F_OK) != 0) {
            if(mkdir(dir, 0775)) {
                syslog(LOG_ERR, "create directory %s faild : %s \n", dir, strerror(errno));
                exit(-1);
            }
        } 
    } else if(action == ACTION_REMOVE) {
        if(access(dir, F_OK) == 0) {
            if(remove(dir)) {
                syslog(LOG_ERR, "remove directory %s faild : %s \n", dir, strerror(errno));
                exit(-1);
            }
        } 
    }
}

static void server_exit()
{
    if(server == NULL)
        return;
    close(server->sfd);
    free(server);
    abspath_diraction(FIFO_SERVER, ACTION_REMOVE);
    abspath_diraction(SERVER_PID, ACTION_REMOVE);
}

static void server_response_by_request(char *request, char *response)
{
    /* for test, just return self. it's unsafe */
    strcpy(response, request);
}

static void server_request_paser(struct server_t *server)
{
    char *ptr;
    long msglen;
    char msgcont[SBUFMAX];

    server->state = SSTATE_INVALID_REQ;
    int msglen_min = strlen(MSG_HEAD) + strlen(MSG_TAIL);
    if(server->rlen <= msglen_min) {
        syslog(LOG_ERR, "server read len invaild : %d \n", server->rlen);
        return ;
    }
    /* MSG_HEAD check */
    if(strncmp(server->sbuf, MSG_HEAD, strlen(MSG_HEAD)) != 0) {
        syslog(LOG_ERR, "client MSG_HEAD check failed \n");
        return ;
    }

    ptr = server->sbuf + strlen(MSG_HEAD);
    /* then get client pid */
    if(sscanf(ptr, "%ld\n",(long *) (&server->curclient.clipid)) <= 0) {
        syslog(LOG_ERR, "client pid catch error \n");
        return ;
    }
    /* get msg len */
    ptr += MSG_PIDBUF_LEN;
    if(sscanf(ptr, "%ld\n", &msglen) <= 0) {
        syslog(LOG_ERR, "get client request msglen error --%s--\n", ptr);
        return ;
    }
    if(msglen <= 0) {
        syslog(LOG_ERR, "invalid msglen : %ld \n", msglen);
        return ;
    }
    syslog(LOG_DEBUG, "msglen=%ld", msglen);
    /* get msg content */
    ptr += MSGLENDESC_BUF_LEN;
    memcpy(msgcont, ptr, msglen);
    msgcont[msglen] = 0;

    /* MSG_TAIL check */
    ptr += msglen;
    if( (strlen(ptr) != strlen(MSG_TAIL)) || 
            (strncmp(ptr, MSG_TAIL, strlen(MSG_TAIL)) != 0) ) {
        syslog(LOG_ERR, "client MSG_TAIL check failed %s\n", ptr);
        return ;
    }
    /* msg paser */
    if(!strcmp(msgcont, REQ_CONN))
        server->state = SSTATE_CONN;
    else if(!strcmp(msgcont, REQ_DISCON))
        server->state = SSTATE_DISCON;
    else {
        server_response_by_request(msgcont, server->curclient.respmsg);
        server->state = SSTATE_RESP;
    }
}

static int server_response_send(pid_t pid, char *resp, int len)
{
    char clifipath[100];
    int fd, ret = 0;

    PATH_BYPID(pid, clifipath);
    fd = open(clifipath, O_WRONLY);
    if(fd < 0)
        return fd;
    if((ret = write(fd, resp, len)) != len) {
        goto client_close;
    }
    
client_close:
    close(fd);
    return ret;
}

static int server_create_conn(pid_t pid)
{
    char clifipath[100];
    int fd = -1;

    PATH_BYPID(pid, clifipath);
    if(access(clifipath, F_OK) == 0)
        return -1;
    if(!mkfifo(clifipath, SERVER_PID_MODE)) {
        syslog(LOG_ERR, "mkfifo %s failed : %s\n", clifipath, strerror(errno));
        return -1;
    }
    while(fd < 0) {
        fd = open(clifipath, O_WRONLY);
    }
    return 0;
}

static int server_destroy_conn(pid_t pid)
{
    char clifipath[100];
    PATH_BYPID(pid, clifipath);
    return remove(clifipath);
}

static void server_handler(struct server_t *server)
{
    switch(server->state) {
        case SSTATE_R:
            server->rlen = read(server->sfd, server->sbuf, SBUFMAX);
            if(server->rlen == 0) {
                server->state = SSTATE_ERR;
                break;
            } else if(server->rlen < 0) {
                if(errno != EAGAIN)
                    server->state = SSTATE_ERR;
                break;
            }
            server->state = SSTATE_PASER;
            syslog(LOG_DEBUG, "%s", server->sbuf);
            break;
        case SSTATE_PASER:
            server_request_paser(server);
            break;
        case SSTATE_CONN:
            if(server_create_conn(server->curclient.clipid))
                server->state = SSTATE_ERR;
            else
                server->state = SSTATE_R;
            break;
        case SSTATE_RESP:
            if(server_response_send(server->curclient.clipid, server->curclient.respmsg, strlen(server->curclient.respmsg)) < 0)
                server->state = SSTATE_ERR;
            else
                server->state = SSTATE_R;
            break;
        case SSTATE_DISCON:
            if(server_destroy_conn(server->curclient.clipid) < 0)
                server->state = SSTATE_ERR;
            else
                server->state = SSTATE_R;
            break;
        case SSTATE_ERR:
            /* do something ..*/
            close(server->sfd);
            abspath_diraction(FIFO_SERVER, ACTION_REMOVE);
            abspath_diraction(SERVER_PID, ACTION_REMOVE);
            server->state = SSTATE_STOP;
            break;
        case SSTATE_INVALID_REQ:
            /* do something */
            server->state = SSTATE_R;
            break;
        default:
            /* unexpected condition */
            break;
    }
}

static void server_start()
{
    syslog(LOG_INFO, "fifo server daemon start !");
    while(server->state != SSTATE_STOP) {
        server_handler(server);
    }
}

static int fifo_daemon_already()
{
    int fd;
    char buf[10];
    
    if((fd = open(SERVER_PID, O_RDWR | O_CREAT, SERVER_PID_MODE)) < 0) {    /* we cannot O_TRUNC, we have not check */
        syslog(LOG_ERR, "open %s failed : %s \n", SERVER_PID, strerror(errno));
        exit(-1);
    }
    if(lock_file(fd) < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "lock file error \n");
        exit(1);
    }
    ftruncate(fd, 0);
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
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d\n", fd0, fd1, fd2);
        exit(-1);
    }
    abspath_diraction(SERVER_PID, ACTION_CREATE);
    abspath_diraction(FIFO_SERVER, ACTION_CREATE);
}

int main(void)
{
    int ret;
    server_envinit();
    if(fifo_daemon_already()) {
        syslog(LOG_ERR, "daemon is already running \n");
        exit(-1);
    }
    if(server == NULL) {
        server = malloc(sizeof(struct server_t));
        if(server == NULL) {
            syslog(LOG_ERR, "server malloc error : %s \n", strerror(ENOMEM));
            exit(-ENOMEM);
        }
    }
    if(access(FIFO_SERVER, F_OK)) {
        if((ret = mkfifo(FIFO_SERVER, 0622)) < 0) {
            syslog(LOG_ERR, "mkfifo %s error : %s \n", FIFO_SERVER, strerror(errno));
            goto server_exit;
        }
    }
    atexit(server_exit);
    server->sfd = open(FIFO_SERVER, O_RDWR | O_CREAT);   /* open by read write to avoid recv end of file */
    server->state = SSTATE_R;
    server_start();

server_exit:
    free(server);
    exit(0);
}
