#if 0
    1 进程会计
        a. 结束进程的会计记录，包括总量较小的二进制数据，一般包括命令名，所使用的CPU时间总量
            用户ID和组ID，启动时间等
        b. accont / /var/account pacct

    2. 用户标识
        b. 一个用户一个登录名，一个用户ID
            struct passwd *getgwuid(getuid());
        a. 一个用户有多个登录名，对应同一个用户ID（一个用户在口令文件可以有多个登录项，用户ID相同，登录shell则不同）
            char *getlogin(void);(守护进程调用会失败)
            struct passwd *getpwnam(const char *name);

    3. 进程时间
        a. clock_t times(struct tms *buf);
            返回值：成功返回流逝的墙上时钟时间（相对值），出错返回-1
            struct tms {
                clock_t tms_utime;  /* user CPU time */
                clock_t tms_stime;  /* system CPU time */
                clock_t tms_cutime; /* user CPU time of children */
                clock_t tms_cstime; /* system CPU timed of childern */
            };
            注：相对值，计算时需差值；单位时钟滴答数
        b. demo_proctimes
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>



#define err_exit(fmat, arg...)                                              \
{                                                                           \
    fprintf(stderr, "[%s]<%d>: "fmat, __FUNCTION__, __LINE__, ##arg);        \
    exit(-1) ;                                                              \
}

static void print_time(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
    /* static ,we just need it one time */
    static long clktck = 0;
    if(clktck == 0) {
        if((clktck = sysconf(_SC_CLK_TCK)) < 0) {
            fprintf(stderr, "get clock ticks per second failed \r\n");
            return ;
        }
    }
    printf("real time: %7.2f\r\n", (double) real/clktck);
    printf("user time: %7.2f\r\n", (double) (tmsend->tms_utime - tmsstart->tms_utime) / clktck);
    printf("system time: %7.2f\r\n", (double) (tmsend->tms_stime - tmsstart->tms_stime) / clktck);
    printf("child real time: %7.2f\r\n", (double) (tmsend->tms_cutime - tmsstart->tms_cutime) / clktck);
    printf("user time: %7.2f\r\n", (double) (tmsend->tms_cstime - tmsstart->tms_cstime) / clktck);
}

static void do_cmd(const char *cmdstring)
{
    clock_t real_start, real_end;
    struct tms tms_start, tms_end;
    if((real_start = times(&tms_start)) < 0)
        err_exit("get tims failed \r\n");
    if(system(cmdstring) < 0)
        err_exit("system(%s) \r\n", cmdstring);
    if((real_end = times(&tms_end)) < 0)
        err_exit("get tims failed \r\n");
    print_time(real_end-real_start, &tms_start, &tms_end);
}

int main(int argc, char *argv[])
{
    int i ;
    for(i = 1; i < argc; i++) {
        do_cmd(argv[i]);
    }
    exit(0);
}
