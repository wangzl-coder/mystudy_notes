#include "token_bucket_lib.h"


typedef struct tokenbct_t
{
    unsigned int token;
    unsigned int tok_ps;
    unsigned int tok_max;
}tokbct_t;

static struct tokenbct_t *tbf_array[TOKEN_BUCKET_MAX_COUNT];

tokenbct_t *token_bucket_init(unsigned int tkps, unsigned int tkmax)
{
    struct tokbct_t *tbf = malloc(sizeof(tokbct_t));
    if(tbf != NULL) {
        tbf->token = 0;
        tbf->tok_ps = tkps;
        tbf->tok_max = tkmax;
        
    }
    return tbf;
}
