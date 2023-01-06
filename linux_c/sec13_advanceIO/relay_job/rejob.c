#include <stdio.h>
#include <errno.h>
#include <unistd.h>


enum jobstate_e{
    JSTATE_R = 0x0,
    JSTATE_W,
    JSTATE_ERR,
    JSTATE_T,
};

struct rejob_desc_t{
    int fd1;
    int fd2;
    unsigned char jobstate;
};

struct job_task_t{
    pthread_t tid;
    pthread_mutex_t job_lock;
    pthread_cond_t nojob_cond;
};

struct rejob_t{
    int count;
    struct job_task_t jtask;
    struct rejob_desc_t *job_desc;
};

static void *thread_rejob(void *arg)
{
    int i;
    struct rejob_t *rejob = arg;
    while(1) {
        pthread_mutex_lock(&retask.job_lock);
        for(i = 0; i <rejob->count; i++) {
            
        }
        pthread_mutex_unlock(&retask.job_lock);
    }

}

struct rejob_t* relay_job_init(int count)
{
    struct rejob_t *rejob;
    struct rlimit rlimt;
    int i;
    int ret;

    if(getrlimit(RLIMIT_NOFILE, &rlimit)) {
        fprintf(stderr, "getrlimit error : %s \r\n", strerror(errno));
        return -1;
    }
    if(count > rlimit.rlim_max) {
        return NULL;
    }
    if(count > rlimit.rlim_cur) {
        rlimit.rlim_cur = count;
        if(setrlimit(RLIMIT_NOFILE, &rlimit)) {
            fprintf(stderr, "setrlimit error : %s \r\n", strerror(errno));
            return -1;
        }
    }
    rejob = malloc(sizeof(struct rejob_t));
    if(rejob == NULL)
        return NULL;
    rejob->count = count;
    rejob->job_desc = malloc(count * sizeof(struct rejob_desc_t));
    if(rejob->job_desc == NULL) {
        free(rejob);
        return NULL;
    }
    for(i = 0; i < rejob->count; i++) {
        rejob->job_desc[i].state = 
    }
    ret = pthread_mutex_init(&rejob->jtask.job_lock, NULL);
    if(ret)
        goto ret_failed;
    ret = pthread_create(&rejob->jtask.tid, NULL, thread_rejob, rejob);
    if(ret)
        goto ret_failed;
    
    return rejob;

ret_failed:
    fprintf(stderr, "pthread_create failed : %s \r\n", strerror(ret));
    free(rejob);
    return NULL;
}



int relay_job_add(int fd1, int fd2)
{
        
}
