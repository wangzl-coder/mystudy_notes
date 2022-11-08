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
        实际用户ID和实际组ID-->实际用户和所属组，标识我们究竟是谁，取自口令文件的登陆项
        有效用户ID，有效组ID，附属组ID-->决定文件访问权限
        保存的设置组ID和设置用户ID-->有效ID的副本，由exec函数族保存
    b.set-user-ID,set-group-ID。st_mode测试量S_ISUID,S_ISGID

4 文件访问权限：
    a. st_mode访问权限位： 用户u，组g和其他o
    b.文件访问权限规则:
        (1) 目录搜索位，目录可执行权限位
        (2) 文件读权限 O_RDONLY O_RDWR
        (3) 文件写操作 O_WRONLY O_RDWR
        (4) O_TRUNC时，必须有写权限
        (5) 目录中创建新文件，对目录须有写权限和执行权限
        (6) 删除文件，须对包含文件的目录具有写和执行权限，对文件本身不需有读写权限
        (7) exec函数族，执行某个文件，文件需有可执行权限
    c.内核测试访问权限策略：
        (1) 进程有效用户ID为0（root）、
        (2) 进程有效ID为为文件所有者ID，所有者访问权限位
        (3) 进程有效组ID（附加组ID）== 文件的组ID，组访问权限位
        (4) 其他用户访问权限位
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
    int chomd(const char *path, mode_t mode);
    int fchmod(int fd, mode_t mode);

9 chown , fchown , 和lchown
    int chown(const char *path, uid_t owner, gid_t group);
    int fchown(int fd, uid_t owner, gid_t group);
    int lchown(const char *path, uid_t owner, gid_t group);

10 文件长度
    (struct stat).st_size;
    a.普通文件：可以为0
    b.目录:一个数的整数
    c.符号链接：指向文件的文件名的实际字节数
    d.文件空洞：du -s, wc -c

11 文件截短
    int truncate(const char *path, off_t length);
    int ftruncate(int fd, off_t length);

12 文件系统
    磁盘： ----------------------------------------------------------   
           |                   |                  |                 | 
           |        分区1      |       分区2      |      ...        | 
           |                   |                  |                 | 
           ----------------------------------------------------------   
                                        ||
                                        ||
                                        ||
                                        \/
文件系统：  -----------------------------------------------------------------
            |       |       |           |           |           |           |
            |自举块 | 超级块|   柱面组1 |   柱面组2 |   柱面组3 |    ...    |
            |       |       |           |           |           |           |
            -----------------------------------------------------------------
                                 ||
                                 ||
                                 ||
                                 \/
            -------------------------------------------------------------------------
            |           |           |           |           |           |           |
            |超级块副本 |  配置信息 |  i节点图  |   块位图  |    i节点  |   数据块  |
            |           |           |           |           |           |           |
            -------------------------------------------------------------------------
                                                                 ||
                                                                 ||
                                                                 ||
                                                                 \/
                                                -------------------------------------------------
                                                |   i节点   |    i节点   |   ...   |    i节点   |
                                                -------------------------------------------------


            -----------------------------------------------------------------------------------------
            | i节点数组 |---| 数据块 |--| 目录快 |-------| 数据块 |---| 目录块|----| 数据块 |--|.....
            -----------------------------------------------------------------------------------------
                ||                          ||                            ||              
                ||                          ||                            ||
                \/                          ||                            ||
-----------------------------------         ||                            ||
| i节点 |---| i节点 |--| i节点 |...         ||                            ||
-----------------------------------         ||                            ||
                                            \/                            \/
                                    ---------------------         ---------------------
                                    | i节点号 |--|文件名|         | i节点号 |--|文件名|
                                    ---------------------         ---------------------

考虑：
    （1） 每个i节点指向一个或多个数据块
    （2） 链接计数：(struct stat).st_nlink目录块中的i节点号指向包含的文件i节点，每个i节点都有链接计数，链接计数为0时，表示删除此文件
    （3） 硬链接：多个i节点号指向同一个文件的i节点，
    （4） 符号链接：文件实际内容（数据块）为指向文件的名字
    （5） i节点信息
    （6） mv实现方式
    （7） 目录文件的链接计数

13 link,unlink,remove,rename
    a. int link(const char *oldpath, const char *newpath);
        创建指向现有文件（oldpath）的链接

    b. int unlink(const char *pathname);
        删除现有的目录项，
        unlink实现临时文件的创建（demo_unlink）

    c. int remove(const char *pathname);
       int rename(const char oldpath, const char *newpath);

14 符号链接：ln -s
    a. 硬链接的限制：不可跨文件系统，超级用户可用
    b. 各函数符号链接的跟随性
    c.符号链接的循环问题

15 symlink和readlink
    a. int symlink(const char *oldpath, const char *newpath);
        创建指向oldpath的符号链接文件newpath
    b.ssize_t readlink(const char *path, char *buf, size_t bufsiz);
        读链接文件本身，组合open，read，close所有操作

16 文件时间：
    a. 三个时间值：
        st_atime: 文件数据最后访问时间，read ，write， ls -lu
        st_mtime: 文件数据最后修改时间， write，
        st_ctime: i节点状态最后修改时间 ，chmod,chown , ls -lc
    
    b. int utime(const char *filename, const struct utimbuf *times);
        更改文件的访问和修改时间(demo_utime)

17 mkdir和rmdir
    a. int mkdir(const char *pathname, mode_t mode);
        创建新的空目录，mode由进程文件模式创建屏蔽字修改。
        至少设置一个执行权限位。
        新目录的用户ID和组ID问题。
    b. int rmdir(const char *pathname);

18 读目录（POSIX标准下的C Library）(demo_filetype_stats)
    a. 目录写权限：只有内核能写，写权限表示能否在该目录创建或删除文件
    b. DIR *opendir(const char *name);
        DIR : 内部维护，类似FILE，用户不关注其结构

    c. struct dirent *readdir(DIR *dirp);
        struct dirent：与具体实现有关，linux的定义：
            struct dirent {
                ino_t          d_ino;       /* inode number */
                off_t          d_off;       /* offset to the next dirent */
                unsigned short d_reclen;    /* length of this record */
                unsigned char  d_type;      /* type of file; not supported
                                       by all file system types */
                char           d_name[256]; /* filename */
            };
    d. void rewinddir(DIR *dirp);
    e. void closedir(DIR *dirp);
    f. long telldir(DIR *);
    g. void seekdir(DIR *, long offset);

19 chdir,fchdir,getcwd(Ex: demo_chdir)
    a. 当前工作目录，用户起始目录
    b. 
        int chdir(const char *path);
        int fchdir(int fd);
        进程属性，仅对调用进程有效，
        char *getcwd(char *buf, size_t size);
        拿到完整工作路径（包括null）
    
20 设备特殊文件
    a. st_dev(文件系统设备号，磁盘) 和 st_rdev(针对字符特殊文件和块特殊文件，实际设备的设备号，driver)
#endif  

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>
#include <limits.h>


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

static void demo_unlink()
{
    if(open("tmpfile", O_CREAT | O_RDWR |O_TRUNC) < 0) {
        fprintf(stderr, "create file failed \r\n");
        return ;
    }
    if(unlink("tmpfile") < 0 )
        fprintf(stderr, "unlink tmpfile error\r\n");
    printf("unlink tmpfile \r\n");
    sleep(15);
}

static int demo_utime(int count, char *path[])
{
    int i;
    int fd;
    struct stat statbuf;
    struct utimbuf timebuf;

    for(i = 0; i < count; i ++) {
        if(stat(path[i], &statbuf) < 0) {
            fprintf(stderr, "%s : stat error \r\n", path[i]);
            continue;
        }
        if((fd = open(path[i], O_RDWR | O_TRUNC)) < 0) {
            fprintf(stderr, "open %s failed ",path[i]);
            perror(NULL);
            continue;
        }
        close(fd);
        timebuf.actime = statbuf.st_atime;
        timebuf.modtime = statbuf.st_mtime;
        if(utime(path[i], &timebuf) < 0) {
            fprintf(stderr, "%s : utime failed \r\n", path[i]);
            continue;
        }
    }
    return 0;
}

static int demo_chdir(void)
{
    int ret;
    char buf[PATH_MAX];
    if(getcwd(buf, PATH_MAX) == NULL) {
        ret = -1;
        fprintf(stderr, "%d -> getcwd failed \r\n",__LINE__);
        return ret;
    }
    printf("current work directory %s \r\n",buf);
    if((ret = chdir("/home/wangzl/workSpace/mystudy_notes/linux_c")) < 0) {
        fprintf(stderr, "%d -> chdir failed \r\n",__LINE__);
        return ret;
    }
    printf("changed work directory to %s \r\n","/home/wangzl/workSpace/mystudy_notes/linux_c");
    if(getcwd(buf, PATH_MAX) == NULL) {
        fprintf(stderr, "%d -> getcwd failed \r\n",__LINE__);
        ret = -1;
    }
    printf("current work directory %s \r\n",buf);
    return ret;
}

static int demo_dev_t(int count, char *path[])
{
    int i;
    struct stat statbuf;
    for(i = 0; i < count; i++) {
        if(stat(path[i], &statbuf) < 0) {
            fprintf(stderr, "%s : stat failed \r\n", path[i]);
            continue;
        }
        puts(path[i]);
        printf("dev : major %d, minor %d \r\n", major(statbuf.st_dev), minor(statbuf.st_dev));
        if(S_ISCHR(statbuf.st_mode) || S_ISBLK(statbuf.st_mode)) {
            printf("(%s) rdev : major %d, minor %d \r\n", (S_ISCHR(statbuf.st_mode)) ? "character" : "block", 
                    major(statbuf.st_rdev), minor(statbuf.st_rdev));
        
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
//  ret = demo_stat_ftype(argc-1, &argv[1]);
//  demo_umask();
//  demo_unlink();
//  demo_utime(argc - 1, &argv[1]);
//  ret = demo_chdir();
    ret = demo_dev_t(argc - 1, &argv[1]);
    exit(ret);
}

