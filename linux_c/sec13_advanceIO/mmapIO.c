#if 0
    1 存储映射IO : 能将一个磁盘文件映射到存储空间的一个缓冲区上，于是，当从缓冲区中取数据时
        就相当于读文件中的相应字节。与此类似，将数据存入缓冲区时，相应字节就自动写入文件，
        这样，就可以在不使用read和write的情况下执行IO

    2 为了使用这种功能，应首先告诉内核将一个给定的文件映射到一个存储区域中：

    void *mmap(void *addr, size_t len, int prot, inf flag, int fd, off_t off);
        @addr: 指定映射存储区的起始地址，通常设置为0，表示由系统选择该映射区的起始地址
        @fd: 要被映射的文件描述符
        @prot: 指定映射存储区的保护要求（除PROT_NONE,其他可按位或）：
            a. PROT_READ -> 映射区可读
            b. PROT_WRITE -> 映射区可写
            c. PROT_EXEC -> 映射区可执行
            d. PROT_NONE -> 映射区不可访问
        @flag:
            a. MAP_FIXED: 返回值必须等于addr，不鼓励使用
            b. MAP_SHARED: 描述本进程对映射区所进行的存储操作的配置。
            c. MAP_PRIVATE: 对映射区的存储操作导致创建该映射文件的一个私有副本（用作调试，因为不改变原文件）

        @off: 通常指定0
        @return: 返回映射区的起始地址

    更改现有映射的权限
    int mprotect(void *addr, size_t len, int prot);
    
    将共享映射的页冲洗到被映射的文件中：
    int msync(void *addr, size_t len, int flags)

    解除映射：
    int munmap(void *addr, size_t len)

#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILE_MODE (0644)
/* 用存储映射IO复制文件 */
#define COPYINCR (1024*1024*1024)  /* 1 GB */

int main(int argc, char *argv[])
{
    int fdin, fdout;
    void *src, *dest;
    size_t copysz;
    struct stat sbuf;
    off_t fsz = 0;
    int ret = 0;

    if(argc != 3) {
        fprintf(stderr, "usage : %s <fromfile><tofile>", argv[0]);
        exit(-1);
    }
    if((fdin = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "can't open %s for reading \r\n", argv[1]);
        exit(-1);
    }
    if((fdout = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0) {
        fprintf(stderr, "can't create %s for writing", argv[2]);
        ret = fdout;
        goto close_infile;
    }
    if((ret = fstat(fdin, &sbuf)) < 0) {
        fprintf(stderr, "fstat failed \r\n");
        goto close_allfile;
    }
    if((ret = ftruncate(fdout, sbuf.st_size)) < 0) {        /* set output file size */
        fprintf(stderr, "ftruncate failed \r\n");
        goto close_allfile;
    
    }
    while(fsz < sbuf.st_size) {
        if((sbuf.st_size - fsz) > COPYINCR)
            copysz = COPYINCR;
        else 
            copysz = sbuf.st_size - fsz;

        if((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED) {
            fprintf(stderr, "mmap error for input \r\n");
            ret = -1;
            goto close_allfile;
        }
        if((dest = mmap(0, copysz, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, fsz)) == MAP_FAILED) {
            fprintf(stderr, "mmap error for output \r\n");
            ret = -1;
            munmap(src, copysz);
            goto close_allfile;
        }
        memcpy(dest, src, copysz);
        munmap(src, copysz);
        munmap(dest, copysz);
        fsz += copysz;
    }

close_allfile:
    close(fdout);
close_infile:
    close(fdin);
    exit(ret);
}
