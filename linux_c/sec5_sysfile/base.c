#if 0
1 口令文件
    a. /etc/passwd 字段用冒号隔开
    注意：
        （1）root用户登陆项
        （2）加密口令包含一个占位字符，不在此存放
        （3）某些字段可以为空
        （4）可执行程序名

    b. 获取口令文件项
        struct passwd *getpwuid(uid_t uid);
        struct passwd *getpwnam(const char *name);
        truct passwd {
            char   *pw_name;       /* username */
            char   *pw_passwd;     /* user password */
            uid_t   pw_uid;        /* user ID */
            gid_t   pw_gid;        /* group ID */
            char   *pw_gecos;      /* user information */
            char   *pw_dir;        /* home directory */
            char   *pw_shell;      /* shell program */
        };
    c. 查看整个口令文件
        struct passwd *getpwent(void);
            返回口令文件下一个记录项，第一次调用会打开文件，
        void setpwent(void);
            反绕文件
        void endpwent(void);
            关闭文件
2 阴影口令：
    a. 加密口令：单向算法级加密处理过程
    b. 阴影口令：加密口令存放文件。仅login()和passwd()程序有权限读取（设置用户ID为root的程序）

3 组文件：
    a. struct group *getgrgid(gid_t gid);
        struct group *getgrnam(const char *name);
        struct group {
            char   *gr_name;       /* group name */
            char   *gr_passwd;     /* group password */
            gid_t   gr_gid;        /* group ID */
            char  **gr_mem;        /* group members */
        };
    b. 与口令文件相似的
        struct group *getgrent(void);

        void setgrent(void);
        void endgrent(void);

4 附加组ID：
    a. 附加组：一个用户可以拥有多个组参与多个项目，进程的有效组ID和附加组ID将和文件组ID比较进行权限检查

5 其他数据文件：
    a. /etc/hosts , /etc/services , /etc/protocols , /etc/networks ..

6 登录账户记录
7 系统标识

8 时间和日期
    a. 日历时间：国际标准时间公元1970年1月1日00:00:00以来经过的秒数。time_t
    time_t time(time_t *t);
    int gettimeofday(struct timeval *tv, struct timezone *tz);
    struct timeval{
        time_t tv_sec; /* seconds */
        suseconds_t tv_usec; /* microseconds */
    };
    b. 时间转换：
        struct tm *gmtime(const time_t *timep);
            国际标准时间
        struct tm *localtime(const time_t *timep);
            本地时间（时区和夏时制标志）

        time_t mktime(struct tm *tm);
        struct tm {
            int tm_sec;         /* seconds */
            int tm_min;         /* minutes */
            int tm_hour;        /* hours */
            int tm_mday;        /* day of the month */
            int tm_mon;         /* month */
            int tm_year;        /* year */
            int tm_wday;        /* day of the week */
            int tm_yday;        /* day in the year */
            int tm_isdst;       /* daylight saving time */
        };
    
    c. 字符串转换：
        char *asctime(const struct tm *tm);
        char *ctime(const time_t *timep);
        size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

static void demo_passwd()
{
    struct passwd *pwd;
    
    /* 保护措施，如果此之前已经使用getpwent打开，则反绕文件到开始处 */
    setpwent();

    printf("%-20s%-20s%-20s%-20s%-20s%-20s%-20s \r\n", "username", "password", "userID", "groupID", "userInfo", "home direc", "shell");
    while((pwd = getpwent()) != NULL) {
        printf("%-20s",pwd->pw_name);
        printf("%-20s",pwd->pw_passwd);
        printf("%-20d",pwd->pw_uid);
        printf("%-20d",pwd->pw_gid);
        printf("%-20s",pwd->pw_gecos);
        printf("%-20s",pwd->pw_dir);
        printf("%-20s",pwd->pw_shell);
        putchar('\n');
    }
    endpwent();
}

static void demo_group()
{
    struct group *gr;
    
    setgrent();
    printf("%-20s%-20s%-20s%-20s \r\n", "groupname", "gid", "passwd", "member");
    while((gr = getgrent()) != NULL) {
        printf("%-20s", gr->gr_name);
        printf("%-20d", gr->gr_gid);
        printf("%-20s", gr->gr_passwd);
        printf("%-20s", gr->gr_mem[0]);
        putchar('\n');
    }
    endgrent();
}

static void demo_time()
{
    time_t cur_time;
    struct tm *tm;
    
    cur_time = time(NULL);
    puts(ctime(&cur_time));
    tm = localtime(&cur_time);
    printf("%-10s: %d \r\n","tm_sec",      tm->tm_sec);
    printf("%-10s: %d \r\n","tm_min",      tm->tm_min);
    printf("%-10s: %d \r\n","tm_hour",     tm->tm_hour);
    printf("%-10s: %d \r\n","tm_mday",     tm->tm_mday);
    printf("%-10s: %d \r\n","tm_mon",      tm->tm_mon);
    printf("%-10s: %d \r\n","tm_year",     tm->tm_year);
    printf("%-10s: %d \r\n","tm_wday",     tm->tm_wday);
    printf("%-10s: %d \r\n","tm_yday",     tm->tm_yday);
    printf("%-10s: %d \r\n","tm_isdst",    tm->tm_isdst);
    puts(asctime(tm));
}

int main()
{
    demo_passwd();
    puts("--------------------------------------------------------------------------------");
    puts("\n\n\n group information:");
    demo_group();
    puts("--------------------------------------------------------------------------------");
    demo_time();
    exit(0);
}
