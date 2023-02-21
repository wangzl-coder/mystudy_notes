#ifndef MY_PIPE_H__
#define MY_PIPE_H__

typedef void mypipe_t;

mypipe_t mypipe_init(int);
int mypipe_read(mypipe_t, char *, int );
int mypipe_write(mypipe_t, char *, int );

#endif
