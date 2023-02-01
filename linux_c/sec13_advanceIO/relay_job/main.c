#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "rejob.h"


#define DEV_1 "/dev/tty3"
#define DEV_2 "/dev/tty4"

int main(void)
{
    int fd1, fd2;
    if((fd1 = open(DEV_1, O_RDWR)) < 0) {
        fprintf(stderr, "open %s failed : %s \r\n", DEV_1, strerror(errno));
        exit(-1);
    }

    if((fd2 = open(DEV_2, O_RDWR)) < 0) {
        fprintf(stderr, "open %s failed : %s \r\n", DEV_2, strerror(errno));
        goto fd1_close;
    }
    if(relay_job_inittask(20) != 0) {
        perror("relay job init failed \r\n");
    }
    printf("add relay job %d - %d \r\n", fd1, fd2);
    if(relay_job_add(fd1, fd2) != 0) {
        perror("relay job add failed \r\n");
    }
    usleep(200000);
    rejob_release();

    close(fd2);
fd1_close:
    close(fd1);
    exit(0);
}
