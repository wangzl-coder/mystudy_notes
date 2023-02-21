#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CHRDEV_FLAGS_READ 1
#define CHRDEV_FLAGS_WRITE 2

/**
 * @brief char dev test app
 * 
 * @param argc 
 * @param argv argv[1] drv_name argv[2] r：从drv读数据 w: 写数据到drv argv[3] 写的数据
 * @return int 
 */
int main(int argc, char *argv[])
{
    char *file_name = argv[1];
    int fd;
    int ret = 0;
    char read_buffer[100];
    int flags = 0;

    if(argc == 3 && !strcmp(argv[2], "r"))
        flags = CHRDEV_FLAGS_READ;
    else if(argc == 4 && !strcmp(argv[2], "w"))
        flags = CHRDEV_FLAGS_WRITE;
    else {
        printf("input param error! \r\n");
        return -1;
    }
    fd = open(file_name, O_RDWR);
    if(fd < 0){
        printf("char dev app --- open dev %s failed \r\n",file_name);
        ret = -1;
        goto open_failed;
    }
    if(flags == CHRDEV_FLAGS_READ){
        memset(read_buffer,0,sizeof(read_buffer));
        ret = read(fd, read_buffer, 50);
        if(ret < 0){
            printf("char dev app ---read from %s failed \r\n",file_name);
            goto read_failed;
        }
        printf("app read from drv: %s \r\n",read_buffer);
    } else if(flags == CHRDEV_FLAGS_WRITE) {
        ret = write(fd, argv[3], strlen(argv[3]));
        if(ret < 0){
            printf("char dev app ---write to %s failed \r\n",file_name);
            goto write_failed;
        }
    }else
        ret = -1;

write_failed:
read_failed:
    close(fd);
open_failed:
    return ret;
}