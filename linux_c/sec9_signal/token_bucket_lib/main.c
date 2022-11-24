#include <stdio.h>
#include <stdlib.h>
#include "token_bucket.h"



int main(int argc, char *argv[])
{
    int ret = 0;
    FILE *fp;
    char buf[1024]; 
    tokbct_t *tbf;
    if(argc != 2) {
        fprintf(stderr, "invaild param \r\n");
        exit(-1);
    }
    tbf = token_bucket_init(10, 100);
    if(tbf == NULL) {
        fprintf(stderr, "token bucket init falied \r\n");
        exit(-1);
    }
    token_bucket_task_active();
    if((fp = fopen(argv[1], "r")) == NULL) {
        perror("open failed");
        exit(-1);
    }
    if(setvbuf(stdout, NULL, _IONBF, 0)) {
        fprintf(stderr, "warnning : set no bufferd for stdout failed \r\n");        
    }
    for(;;) {
        int len;
        char *ptw;
        int fetch = token_bucket_fetch(tbf, 20);
        if((len = fread(buf, 1, fetch, fp)) <= 0) {
            if(ferror(fp)) {
                ret = -1;
                fprintf(stderr, "read %s failed \r\n", argv[1]);
            }
            break;
        }
        if(len < fetch) {
            token_bucket_put(tbf, fetch-len);
        }
        ptw = buf;
        while(len > 0) {
            int wrlen;
            if((wrlen = fwrite(ptw, 1, len, stdout)) <= 0) {
                fprintf(stderr, "write to stdout failed \r\n");
                ret = -1;
                break;
            }
            len -= wrlen;
            ptw += wrlen;
        }
    }

    fclose(fp);
    token_bucket_destory(tbf);
    exit(ret);
}
