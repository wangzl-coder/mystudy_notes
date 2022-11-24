#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include "token_bucket.h"

struct tokenbct_t
{
    unsigned int token;
    unsigned int tok_ps;
    unsigned int tok_max;
    unsigned int index;
};

static struct tokenbct_t *tbf_array[TOKEN_BUCKET_MAX_COUNT];

void (*sigalrm_handler_reserved)(int);

static int free_position(void)
{
    int i;
    for(i = 0; i < TOKEN_BUCKET_MAX_COUNT; i++) {
        if(tbf_array[i] == NULL) {
            break;
        }
    }
    return i;
}

static void tbf_sigalrm_handler(int signum)
{
    int i;
    alarm(1);
    for(i = 0; i < TOKEN_BUCKET_MAX_COUNT; i++) {
        struct tokenbct_t *tbf = tbf_array[i];
        if(tbf != NULL) {
            if((tbf->token += tbf->tok_ps) > tbf->tok_max)
                tbf->token = tbf->tok_max;
        }
    }
}

void token_bucket_task_kill()
{
    int i;
    signal(SIGALRM, sigalrm_handler_reserved);
    for(i = 0; i < TOKEN_BUCKET_MAX_COUNT; i++) {
        if(tbf_array[i] != NULL) {
            free(tbf_array[i]);
            tbf_array[i] = NULL;
        }
    }
}

void token_bucket_task_active()
{
    sigalrm_handler_reserved = signal(SIGALRM, tbf_sigalrm_handler);
    alarm(1);

    atexit(token_bucket_task_kill);
}

tokbct_t *token_bucket_init(unsigned int tkps, unsigned int tkmax)
{
    int free_index;
    struct tokenbct_t *tbf;
    if((free_index = free_position()) >= TOKEN_BUCKET_MAX_COUNT)
        return NULL;

    tbf = malloc(sizeof(struct tokenbct_t));
    if(tbf != NULL) {
        tbf->token = 0;
        tbf->tok_ps = tkps;
        tbf->tok_max = tkmax;
        tbf->index = free_index;
        tbf_array[free_index] = tbf;
    }
    return tbf;
}

int token_bucket_fetch(tokbct_t *tbf, unsigned int count)
{
    int fetch;
    struct tokenbct_t *tokenbuf = tbf;
    if(count == 0)
        return -EINVAL;
    while(!tokenbuf->token) {
        pause();
    }

    fetch = (count > tokenbuf->token)? tokenbuf->token : count;
    tokenbuf->token -= fetch;
    return fetch;
}

int token_bucket_put(tokbct_t *tbf, unsigned int count)
{
    struct tokenbct_t *ptbf = tbf;
    int freecount = ptbf->tok_max - ptbf->token;
    int putcount = (freecount > count)? count: freecount ;
    ptbf->token += putcount;
    return putcount;
}

void token_bucket_destory(tokbct_t *tbf)
{
    struct tokenbct_t *ptbf = tbf;
    tbf_array[ptbf->index] = NULL;
    free(ptbf);
}
