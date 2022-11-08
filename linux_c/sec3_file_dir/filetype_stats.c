#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


typedef int myfunc_ptr(const char *,const struct stat *, int);

static myfunc_ptr myfunc;
static int myftw(char *, myfunc_ptr *);
static int dopath(myfunc_ptr *);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;


int main(int argc, char *argv[])
{
    int ret;

    if(argc != 2) {
        fprintf(stderr, "invaild param \r\n");
        exit(-1);
    }

    ret = myftw(argv[1], myfunc);
    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    if(ntot == 0)
        ntot = 1;

    printf("%-20s= %7ld,     %5.2f %%\n",    "regular files",    nreg,   nreg*100.0/ntot);
    printf("%-20s= %7ld,     %5.2f %%\n",    "directories",      ndir,   ndir*100.0/ntot);
    printf("%-20s= %7ld,     %5.2f %%\n",    "block special",    nblk,   nblk*100.0/ntot);
    printf("%-20s= %7ld,     %5.2f %%\n",    "char special",     nchr,   nchr*100.0/ntot);
    printf("%-20s= %7ld,     %5.2f %%\n",    "FIFOS",            nfifo,  nfifo*100.0/ntot);
    printf("%-20s= %7ld,     %5.2f %%\n",    "symbolic links",   nslink, nslink*100.0/ntot);
    printf("%-20s= %7ld,     %5.2f %%\n",    "sockets",          nsock,  nsock*100.0/ntot);

    exit(ret);
}

#define FTW_F 1     /* 非目录文件 */
#define FTW_D 2     /* 目录文件 */
#define FTW_DNR 3   /* 目录打开，读失败 */
#define FTW_NS  4   /* 目录stat失败 */

static char full_path[PATH_MAX];    /* 每个目录或文件的完整路径 */
static int myftw(char *pathname, myfunc_ptr *func)
{
    strncpy(full_path, pathname, PATH_MAX);
    return dopath(func);
}

/*
 * 对每个目录递归，具体操作由func指定
 * */
static int dopath(myfunc_ptr *func)
{
    int ret;
    struct stat statbuf;
    DIR *dp;
    struct dirent *dirp;
    char *ptr;

    if(lstat(full_path, &statbuf) < 0)              /* 使用lstat,防止符号链接跟随 */
        return func(full_path, &statbuf, FTW_NS);
    if(!S_ISDIR(statbuf.st_mode))                   /* 非目录 */
        return func(full_path, &statbuf, FTW_F);
    
    /* 是directory.先回调func */
    if((ret = func(full_path, &statbuf, FTW_D)) < 0)
        return ret;
    
    ptr = full_path + strlen(full_path);    //构建路径string
    *ptr++ = '/';
    *ptr = 0;

    /* 打开目录 */
    if((dp = opendir(full_path)) == NULL)       /* 打开失败，回调 */
        return func(full_path, &statbuf, FTW_DNR);
    
    /* 读目录 */
    while((dirp = readdir(dp)) != NULL) {
        if(strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0) {
                continue;
        }
        strcpy(ptr, dirp->d_name);
        if((ret = dopath(func)) != 0)   //递归当前目录项
            break;
    }
    ptr[-1] = 0;    /* 清除本次递归路径（回到上级目录） */
    if(closedir(dp) < 0)
        fprintf(stderr, "close directroy failed \r\n",full_path);
    return ret;
}
static int myfunc(const char *pathname, const struct stat *pstat, int type)
{
    switch(type) {
        case FTW_F:
            switch(pstat->st_mode & S_IFMT) {
                case S_IFREG:   nreg++;     break;
                case S_IFBLK:   nblk++;     break;
                case S_IFCHR:   nchr++;     break;
                case S_IFIFO:   nfifo++;    break;
                case S_IFLNK:   nslink++;    break;
                case S_IFSOCK:  nsock++;    break;
                case S_IFDIR:
                    fprintf(stderr, "S_IFDIR for %s \r\n", pathname);
            }
            break;
        case FTW_D:
            ndir++;
            break;
        case FTW_DNR:
            fprintf(stderr, "cannot read directory %s \r\n",pathname);
            break;
        case FTW_NS:
            fprintf(stderr, "stat error for %s \r\n", pathname);
            break;

        default:
            fprintf(stderr, "unknown type %d for pathname %s \r\n", type, pathname);
            break;
    }
    return 0;
}

