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
    b.文件类型：目录文件，普通文件，块设备文件，字符特殊文件，FIFO，符号链接（Ex: demo_stat_ftype）

#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
{
    int ret = 0;
    ret = demo_stat_ftype(argc-1, &argv[1]);
    exit(ret);
}

