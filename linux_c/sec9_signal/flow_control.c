#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define LENTH_PER_SECOND 20

static int flag;

static void sigalrm_handler(int signum)
{
    alarm(1);
    flag = 1;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int ret = 0;
    int len;
    char buf[LENTH_PER_SECOND];
    if(argc != 2) {
        fprintf(stderr, "invaild param \r\n");
        exit(-1);
    }
    if((fp = fopen(argv[1], "r")) == NULL) {
        perror("open failed ");
        exit(-1);
    }
    signal(SIGALRM, sigalrm_handler);
    alarm(1);
    while((len = fread(buf, 1, LENTH_PER_SECOND, fp)) > 0) {
        flag = 0;
        char *ptr = buf;
        while(len > 0) {
            int wlen = fwrite(ptr, 1, len, stdout);
            if(wlen > 0) {
                len -= wlen;
                ptr += wlen;
                fflush(stdout);
            } else if (ferror(stdout)) {
                fprintf(stderr, "fwrite to stdout error \r\n");
                ret = -1;
                break;
            }
        }
        if(ret < 0) break;
        while(!flag) pause();
    }
    if(ferror(fp)) {
        perror("read failed \r\n");
        ret = -1;
    }
    fclose(fp);
    exit(ret);
}
