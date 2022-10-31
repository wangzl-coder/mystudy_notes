#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * int open(const char *path, int oflag, ...(mode_t mode))    
 * 常用oflag:
 * O_RDONLY, O_WRONLY, O_RDWR,
 * O_APPEND, O_CLOEXEC, O_CREAT, O_DIRECTORY, O_EXCL, O_NOCTTY, O_NOFOLLOW, O_NONBLOCK, O_SYNC, O_TRUNC, O_TTY_INIT
 *
 * */

/*
 * int openat(int fd, const char *path, int oflag, ...(mode_t mode))
 * 与open区别：
 *      1 path是绝对路径名，fd被忽略，相当于open
 *      2 path相对路径名，fd指出相对路径名所在fs的起始地址，fd通过打开path所在的目录获得
 *      3 path指定相对路径名，fd具有特殊值AT_FDCWD,此时路径名在当前工作目录获取
 *
 * 解决两个问题：
 *      1 可以使用相对路径名打开目录中的文件
 *      2 可以避免time-of-check-to-time-of-use(TOCTTOU)
 *
 * */

/*
 *  create(const char *path, mode_t mode);
 *  缺陷:只能以读写方式创建
 *
 * */


/*
 *  close(int fd)
 *      进程终止时，内核会自动关闭打开的文件，个别程序可以不显式的close()关闭文件
 *
 * */

/*
 *  off_t lseek(int fd, off_t offset, int whence)
 *
 *  whence--SEET_SET: 文件开始出offset个字节，offset非负整数
 *  whence--SEET_CUR: 文件当前位置offset个字节，offset可正可负
 *  whence--SEET_END: 文件结尾出offset个字节，offset可正可负
 *  
 *  return: 成功返回新的偏移量（长整型）
 *      fd为PIPE,FIFO,Socket时，返回-1，errno = ESPIPE
 *  
 *  某些特殊设备文件可能会出现curr_offset负值的情况，所以不能测试其返回值 < 0，而是 == -1
 *  lseek不引起I/O，只是内核中的记录
 *  off_t 字节数可由_FILE_OFFSET_BITS 指定，默认值8bytes
 * */
static off_t get_curr_offset(int fd)
{
    return lseek(fd, 0, SEEK_CUR);
}


/*
 * ./base < /etc/passwd --> seek OK (nomal file)
 *  cat < /etc/passwd | ./base -->cannot seek (pipe)
 *
 * */
static void stdin_lseek(void)
{
    if(lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
        puts("cannot seek");
    else
        puts("seek OK");
}

/*
 * 空洞文件：
 * 偏移量大于文件长度，
 *      此时写入文件将被加长，结尾与新位置出形成空洞，不占用磁盘
 *
 * */
static int vacucous_file()
{
    int fd;
    int len;
    char buff[] = "hello world";
    int ret = 0;

    fd = open("file.vacucous", O_WRONLY | O_CREAT | O_TRUNC, 0664); /* 11 */
    if(fd < 0) {
        perror("create failed ");
        return -1;
    }
    if(write(fd, buff, strlen(buff)) < 0){
        puts("write failed");
        ret = -1;
        goto close_file;
    }

    if(lseek(fd, 10, SEEK_CUR) == -1) {                          /* 11 + 10 */
        puts("lseek failed");
        goto close_file;
    }
    if(write(fd, buff, strlen(buff)) < 0){                      /* 11 + 10 + 11 */
        puts("write failed");
        ret = -1;
    }
        
close_file:
    close(fd);
    return ret;
}

/*
 * int read(int fd, char *buff, unsigned mbytes)
 * sszie_t read(int fd, void *buff, size_t mbytes)（POSIX.1修改版本）
 *  返回值：读到的字节数，若到达文件尾，返回0，出错返回-1
 *
 * ssize_t write(int fd, const void *buff, size_t mbytes)
 *  返回值:写入字节数，出错返回-1
 *  通常与mbytes相同，否则表示出错，例如磁盘写满或超过给定进程的文件长度限制
 * */

/* IO效率：
 * 以下程序注意点：
 *      1 从标准输入读，写到标准输出，可用重定向向文件读写 ./base < file.vacucous > file.vacucous.backup
 *      2 程序终止时，内核会关闭进程打开的fd，故不考虑关闭文件
 *      3 对kernel而言，文本文件和二进制文件并无区别，所以对这两种文件有效
 *
 * 效率问题：改变BUFFSIZE，time查看其运行时间
 * 预读取(read ahead)技术
 * */
#define BUFFSIZE 4096
static void file_copy()
{
    int n;
    char buf[BUFFSIZE];

    while((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
        if(write(STDOUT_FILENO, buf, n) != n)
            fprintf(stderr, "write failed");
    
    if(n < 0)
        fprintf(stderr, "read failed");
}


int main()
{

//    stdin_lseek();
    vacucous_file();
    file_copy();
    exit(0);
}
