#include <stdio.h>
#include <errno.h>
#include <string.h>


int main(void)
{
    int ret = 0;
    FILE *temp, *src;
    printf("errno is %d \r\n", errno);
    src = fopen("error.c", "r");
    if(src == NULL) {
        printf("errno is %d \r\n", errno);
        perror("open error.c failed ");
        return -1;
    }

    temp = fopen("temp.txt", "r");
    if(temp == NULL) {
        printf("errno is %d \r\n", errno);
        fprintf(stderr, "open temp.txt failed %s \r\n", strerror(errno));    //必须添加string.h头文件,否则编译警告，运行出现段错误
        ret = -1;
        goto close_src;
    }
     
    fclose(temp);
close_src:
    fclose(src);
    return ret;
}
