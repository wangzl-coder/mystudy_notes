#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "rejob.h"


enum jobstate_e{
    JSTATE_NONE = 0x0,
    JSTATE_R,
    JSTATE_W,
    JSTATE_ERR,
    JSTATE_T,
};

struct rejob_desc_t{
    int sfd;
    int dfd;
    unsigned char jobstate;
    char buf[RELY_BUFSIZE];
    char *ptr;
    int len;
    char errstr[100];
};

struct job_task_t{
    pthread_t tid;
    pthread_mutex_t job_lock;
    pthread_cond_t job_cond;
    int exitflag;
};

struct rejob_t{
    int total;
    int vaildcnt;
    struct job_task_t jtask;
    struct rejob_desc_t (*job_desc)[2];
};

static struct rejob_t *rejob;

static int relay_driver(struct rejob_desc_t *relay)
{
    int wlen;
    switch(relay->jobstate) {
        case JSTATE_R:
            relay->len = read(relay->sfd, relay->buf, RELY_BUFSIZE);
            if(relay->len == 0)
                relay->jobstate = JSTATE_R;     // read nothing
            else if(relay->len < 0) {
                if(errno != EAGAIN) {     // otherwise we read again
                    relay->jobstate = JSTATE_ERR;
                    snprintf(relay->errstr, 100, "read fd %d failed: %s\n", relay->sfd, strerror(errno));
                }
            } else {
                printf("from %d ,read len %d", relay->sfd, relay->len);
                relay->ptr = relay->buf;
                relay->jobstate = JSTATE_W;
            }
            break;
        case JSTATE_W:
            wlen = write(relay->dfd, relay->ptr, relay->len);
            printf("wlen %d \r\n", wlen);
            if(wlen < 0) {
                if(errno != EAGAIN) {     // otherwise we write again
                    relay->jobstate = JSTATE_ERR;
                    snprintf(relay->errstr, 100, "write fd %d failed: %s\n", relay->dfd, strerror(errno));
                }
            } else {
                if(wlen < relay->len) {
                    relay->len -= wlen;
                    relay->ptr += wlen;
                } else {
                    printf("to %d ,read len %d", relay->dfd, wlen);
                    relay->jobstate = JSTATE_R;
                }
            }
            break;
        case JSTATE_ERR:
            perror(relay->errstr);
        case JSTATE_T:
            relay->jobstate = JSTATE_NONE;
            break;
        case JSTATE_NONE:
        default:
            break;
    }
}

static int relayjob_handler(struct rejob_t *rejob)
{
    int i;
    for(i = 0; i < rejob->total; i++) {
        struct rejob_desc_t *relay1to2 = &rejob->job_desc[i][0];
        struct rejob_desc_t *relay2to1 = &rejob->job_desc[i][1];
        if(relay1to2->jobstate != JSTATE_NONE)
            relay_driver(relay1to2);
        if(relay2to1->jobstate != JSTATE_NONE)
            relay_driver(relay2to1);
    }
}

static void *thread_rejob(void *arg)
{
    int i;
    struct rejob_t *rejob = arg;
    while(1) {
        pthread_mutex_lock(&rejob->jtask.job_lock);
        while(rejob->vaildcnt <= 0) {
            pthread_cond_wait(&rejob->jtask.job_cond, &rejob->jtask.job_lock);
        }
        if(rejob->jtask.exitflag) 
            pthread_exit((void *) 0);
        relayjob_handler(rejob);
        pthread_mutex_unlock(&rejob->jtask.job_lock);
    }
}

int relay_job_inittask(int total)
{
    struct rlimit rlimt;
    int i;
    int ret = 0;
    int fdtotal = total*2;

    if(getrlimit(RLIMIT_NOFILE, &rlimt)) {
        fprintf(stderr, "getrlimit error : %s \r\n", strerror(errno));
        return -errno;
    }
    /* we limit our max job total: a half of fd counts that support max */
    if(fdtotal > rlimt.rlim_max) {
        return -EINVAL;
    }
    if(fdtotal > rlimt.rlim_cur) {
        rlimt.rlim_cur = fdtotal;
        if(setrlimit(RLIMIT_NOFILE, &rlimt)) {
            fprintf(stderr, "setrlimit error : %s \r\n", strerror(errno));
            return -errno;
        }
    }
    rejob = malloc(sizeof(struct rejob_t));
    if(rejob == NULL)
        return -ENOMEM;
    rejob->total = total;
    rejob->vaildcnt = 0;
    rejob->job_desc = malloc(total * sizeof(rejob->job_desc));
    if(rejob->job_desc == NULL) {
        free(rejob);
        return -ENOMEM;
    }
    for(i = 0; i < rejob->total; i++) {
        rejob->job_desc[i][0].jobstate = JSTATE_NONE;
        rejob->job_desc[i][1].jobstate = JSTATE_NONE;
    }
    rejob->jtask.exitflag = 0;
    ret = pthread_mutex_init(&rejob->jtask.job_lock, NULL);
    if(ret)
        goto mutex_faild;
    ret = pthread_cond_init(&rejob->jtask.job_cond, NULL);
    if(ret) {
        goto cond_failed;
    }
    ret = pthread_create(&rejob->jtask.tid, NULL, thread_rejob, rejob);
    if(ret)
        goto thread_failed;
    
    return 0;

thread_failed:
    pthread_cond_destroy(&rejob->jtask.job_cond);
cond_failed:
    pthread_mutex_destroy(&rejob->jtask.job_lock);
mutex_faild:
    free(rejob->job_desc);
    free(rejob);
    return ret;
}

void rejob_release()
{
    int *retval;
    pthread_mutex_lock(&rejob->jtask.job_lock);
    rejob->jtask.exitflag = 1;
    if(rejob->vaildcnt == 0) {
        rejob->vaildcnt = 1;
        pthread_cond_signal(&rejob->jtask.job_cond);
    }
    pthread_mutex_unlock(&rejob->jtask.job_lock);
    pthread_join(rejob->jtask.tid, (void **)&retval);
    printf("task exit status %ld \r\n", (long int)retval);
    pthread_mutex_destroy(&rejob->jtask.job_lock);
    pthread_cond_destroy(&rejob->jtask.job_cond);
    free(rejob->job_desc);
    free(rejob);
    return ;
}

static int rejob_find_free(struct rejob_t *rejob)
{
    int i;
    for(i = 0; i < rejob->total; i++) {
        if(rejob->job_desc[i][0].jobstate == JSTATE_NONE &&
                rejob->job_desc[i][1].jobstate == JSTATE_NONE)
            break;
    }
    return ((i == rejob->total) ? (-ENOSPC) : i);
}

int relay_job_add(int fd1, int fd2)
{
    int index, ret = 0;
    int sigflag = 0;
    pthread_mutex_lock(&rejob->jtask.job_lock);   
    if((index = rejob_find_free(rejob)) >= 0) {
        int flag1, flag2;
        flag1 = fcntl(fd1, F_GETFL);
        flag2 = fcntl(fd2, F_GETFL);
        fcntl(fd1, F_SETFL, flag1 | O_NONBLOCK);
        fcntl(fd2, F_SETFL, flag2 | O_NONBLOCK);
        rejob->job_desc[index][0].sfd = fd1;
        rejob->job_desc[index][0].dfd = fd2;
        rejob->job_desc[index][0].jobstate = JSTATE_R;
        rejob->job_desc[index][1].sfd = fd2;
        rejob->job_desc[index][1].dfd = fd1;
        rejob->job_desc[index][1].jobstate = JSTATE_R;
        sigflag = !rejob->vaildcnt;
        rejob->vaildcnt ++;
        if(sigflag)
            pthread_cond_signal(&rejob->jtask.job_cond);
    } else
        ret = index;
    pthread_mutex_unlock(&rejob->jtask.job_lock);   
    return ret;
}
