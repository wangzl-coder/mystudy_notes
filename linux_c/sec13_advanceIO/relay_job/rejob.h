#ifndef RELAY_JOB_H__
#define RELAY_JOB_H__

#define RELY_BUFSIZE (1024)

int relay_job_inittask(int total);

int relay_job_add(int fd1, int fd2);

void rejob_release();

#endif
