#ifndef TOKEN_BUCKET_LIB_H__
#define TOKEN_BUCKET_LIB_H__

#define TOKEN_BUCKET_MAX_COUNT 1024


typedef void tokbct_t;

tokbct_t *token_bucket_init(unsigned int, unsigned int);

int token_bucket_fetch(tokbct_t *, unsigned int);

int token_bucket_put(tokbct_t *, unsigned int);

void token_bucket_destory(tokbct_t *);


#endif
