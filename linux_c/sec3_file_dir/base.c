#if 0
1 stat,fstat,lstat
  a.原型
    int stat(const char *path, struct stat *buf);
    int fstat(int fd, struct stat *buf);
    int lstat(const char *path, struct stat *buf);

    struct stat {
        dev_t     st_dev;     /* ID of device containing file */
        ino_t     st_ino;     /* inode number */
        mode_t    st_mode;    /* protection */
        nlink_t   st_nlink;   /* number of hard links */
        uid_t     st_uid;     /* user ID of owner */
        gid_t     st_gid;     /* group ID of owner */
        dev_t     st_rdev;    /* device ID (if special file) */
        off_t     st_size;    /* total size, in bytes */
        blksize_t st_blksize; /* blocksize for file system I/O */
        blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
        time_t    st_atime;   /* time of last access */
        time_t    st_mtime;   /* time of last modification */
        time_t    st_ctime;   /* time of last status change */
    };
2 文件类型：目录文件，普通文件，块设备文件，字符特殊文件，FIFO，符号链接（Ex: demo_stat_ftype）

3 设置用户ID和设置组ID
    a.与进程相关ID：
        实际用户ID和实际组ID-->实际用户和所属组，取自口令文件的登陆项
        有效用户ID，有效组ID，附属组ID-->决定文件访问权限
        保存的设置组ID和设置用户ID-->有效ID的副本，

4 文件访问权限：
    a. st_mode访问权限位： 用户，组和其他
    b.文件访问权限规则:
        (1) 目录搜索位，目录可执行权限位
        (2) 文件读权限 O_RDONLY O_RDWR
        (3) 文件写操作 O_WRONLY O_RDWR
        (4) O_TRUNC时，必须有写权限
        (5) 目录中创建新文件，对目录须有写权限和执行权限
        (6) 删除文件，须对包含文件的目录具有写和执行权限，对文件本身不需有读写权限
        (7) exec函数族，执行某个文件，文件需有可执行权限
    c.内核测试访问权限：
5 新文件和目录所有权
    a. 新文件组ID可以是进程的有效组ID
    b. 新文件的组ID可以是他所在目录的组ID

6 函数 access和 faccessat :按实际用户ID和实际组ID测试访问权限
    int access(const char *pathname, int mode);
    int faccessat(int fd, const char *pathname, int mode, int flag);
    mode: R_OK, W_OK, X_OK
7 函数umask：为进程设置文件模式创建屏蔽字(demo_umask)
    mode_t umask(mode_t cmask);

8 函数chmod，fchmod和fchmoddat

#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
static int demo_stat_ftype(int filenum, char *path[])
{
    int ret;
    struct stat buf;
    int i;

    for(i = 0; i < filenum; i++){
       if(lstat(path[i], &buf) < 0) {
            fprintf(stderr, "%s: lstat failed \r\n",path[i]);
            continue;
       }
       printf("%s file type :", path[i]);
       if(S_ISREG(buf.st_mode))
           printf("normal file\r\n");
       else if(S_ISDIR(buf.st_mode))
           printf("directory\r\n");
       else if(S_ISCHR(buf.st_mode))
           printf("char special file\r\n");
       else if(S_ISBLK(buf.st_mode))
           printf("block special file\r\n");
       else if(S_ISFIFO(buf.st_mode))
           printf("fifo file\r\n");
       else if(S_ISLNK(buf.st_mode))
           printf("symbol linked file\r\n");
       else if(S_ISSOCK(buf.st_mode))
           printf("socket file\r\n");
       else
           fprintf(stderr, "unknown type");
    }
}

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
static void demo_umask(void)
{
    umask(0);
    if(creat("foo", RWRWRW) < 0)
        fprintf(stderr, "create foo failed \r\n");
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(creat("bar", RWRWRW) < 0)
        fprintf(stderr, "create bar failed \r\n");
}

int main(int argc, char *argv[])
{
    int ret = 0;
    //ret = demo_stat_ftype(argc-1, &argv[1]);
    demo_umask();
    exit(ret);
}

