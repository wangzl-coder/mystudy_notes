#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "ftp_proto.h"


#define FTP_SERVER_LOG      "myftp_server"
#define FTP_SPID_FILE       "/tmp/myftp/myftp.pid"
#define FTP_PIDFILE_MODE    (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FTP_TMPFILE_MODE    (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define lock_file(fd) ({         \
    struct flock flck;          \
    flck.l_type = F_WRLCK;      \
    flck.l_start = 0;           \
    flck.l_whence = SEEK_SET;   \
    flck.l_len = 0;             \
    fcntl(fd, F_SETLK, &flck);  \
})

#define unlock_file(fd) ({      \
    struct flock flck;          \
    flck.l_type = F_UNLCK;      \
    flck.l_start = 0;           \
    flck.l_whence = SEEK_SET;   \
    flck.l_len = 0;             \
    fcntl(fd, F_SETLK, &flck);  \
})

static int ftp_server_sndcont(ftp_msg_t *svermsg, int msgid)
{
    FILE *fp;
    char *path = svermsg->pathreq.fpath;
    int ret = 0;
    char unusedval = 0;
    if(path == NULL)
        return -EINVAL;
    if((fp = fopen(path, "r")) == NULL) {
        syslog(LOG_ERR, "failed to open %s: %s\n", path, strerror(errno));
        return -EINVAL;
    }
    svermsg->msgtype = FTP_MSG_CONT;
    while((svermsg->respcont.flen = fread(svermsg->respcont.fcont, 1, FCONTMAX, fp)) > 0) {
        if((ret = msgsnd(msgid, svermsg->respcont.fcont, sizeof(struct file_cont_st) - sizeof(long), 0)) != 0) {
            syslog(LOG_ERR, "msgsnd failed: %s\n", strerror(errno));
            break;
        }
    }
    if(ret || ferror(fp)) {
        svermsg->msgtype = FTP_MSG_ERR;
    } else if(feof(fp)) {
        svermsg->msgtype = FTP_MSG_EOF;
    } else {
        syslog(LOG_ERR, "Warnning: Unknow Result! \n");
        return -EINVAL;
    }
    ret = msgsnd(msgid, &unusedval, sizeof(char), 0);
    if(ret) {
        syslog(LOG_ERR, "msgsnd type %ld failed : %s\n", svermsg->msgtype, strerror(errno));
    }
    return ret;
}

static int ftp_server_start()
{
    key_t key;
    int msgid;
    ftp_msg_t svermsg;
    int ret = 0;
    
    key = ftok(FTP_KEYPATH, FTP_KEYPROJ);
    if(key < 0) {
        syslog(LOG_ERR, "file to key(%s,%d) error: %s\n", FTP_KEYPATH, FTP_KEYPROJ, strerror(errno));
        return(key);
    }
    msgid = msgget(key, O_CREAT | FTP_TMPFILE_MODE);
    if(msgid < 0) {
        syslog(LOG_ERR, "msgget error \n");
        return(msgid);
    }
    while((ret = msgrcv(msgid, &svermsg.pathreq, sizeof(svermsg) - sizeof(long), FTP_MSG_PATH, 0)) >= 0) {
        ret = ftp_server_sndcont(&svermsg, msgid);
        if(ret)
            break;
    }
    syslog(LOG_ERR, "msgrcv error: %s\n", strerror(errno));
    return ret;
}

static int ftp_daemon_alreay()
{
    int fd;
    char buf[10];

    if((fd = open(FTP_SPID_FILE, O_RDWR | O_CREAT, FTP_PIDFILE_MODE)) < 0) {        /* we cannot O_TRUNC, we have not check */
        syslog(LOG_ERR, "open %s failed : %s \n", FTP_SPID_FILE, strerror(errno));
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
    write(fd, buf,strlen(buf));
    return(0);
}

static void ftp_server_envinit(void)
{
    struct rlimit rlimt;
    struct sigaction act;
    pid_t pid;
    int fd0, fd1, fd2;
    int i;

    umask(0);   //set umask to zero
    if(getrlimit(RLIMIT_NOFILE, &rlimt) < 0) {
        perror("cannot get fd limit \n");
        exit(-1);
    }
    if((pid = fork()) < 0) {
        perror("fork error \n");
        exit(-1);
    } else if(pid != 0) {
        exit(0);
    }
    setsid();
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGHUP, &act, NULL) < 0) {
        perror("sigaction SIGHUP error \n");
        exit(-1);
    }

    if((pid = fork()) < 0) {
        perror("fork failed \n");
        exit(-1);
    } else if(pid != 0) {
        exit(0);
    }
    if(chdir("/") < 0) {
        perror("chdir to root failed \n");
        exit(-1);
    }
    if(rlimt.rlim_max == RLIM_INFINITY)
        rlimt.rlim_max = 1024;
    for(i = 0; i < rlimt.rlim_max; i++) {
        close(i);
    }
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    openlog(FTP_SERVER_LOG, LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "my ftpserver log ! \n");
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d \n", fd0, fd1, fd2);
        exit(-1);
    }
}

int main(void)
{
    ftp_server_envinit();
    if(ftp_daemon_alreay()) {
        syslog(LOG_ERR, "server has already running \n");
        exit(-1);
    }
    ftp_server_start();
    exit(0);
}
