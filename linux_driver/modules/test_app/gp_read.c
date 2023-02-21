#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdlib.h>

static pthread_t thread1;
static pthread_t thread2;

static void* task_write(void *p_fd)
{
    char *buff = "1";
    unsigned int count = 1000;
    while(count--){
        write(*(int*)p_fd, buff, sizeof(buff));
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    char *file_name;
    char buff[10];
    int fd;
    if(argc != 2)
        return -1;

    file_name = argv[1];
    fd = open(file_name, O_RDONLY);
    read(fd, buff, sizeof(buff));
    printf("current value is %s \r\n",buff);

    return 0;
}