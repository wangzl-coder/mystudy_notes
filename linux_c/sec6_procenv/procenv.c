#if 0
1 命令行参数：
    int main(int argc, char *argv[])
        argv[argc] == NULL;

2 环境表
    extern char **environ;
    字符指针数组地址

3 C程序存储空间布局
    a. text（代码，正文）段：机器指令，只有一个副本，只读共享
    b. data（初始化数据段）：明确的初始值，int data = 100; 注：全局声明
    c. bss（非初始化数据段）：不明确的初始值，执行前由内核清0或NULL。long sum[1000] 注：全局声明
    d. stack（栈）：自动变量，函数调用时的现场保存，参数传递，调用返回。多任务调度
    e. heap（堆）：动态存储分配

4 静态库和共享库
    a. 静态库：用于模块的封装和隐藏，编译时链接，
        创建：gcc -c *.c -o *.o ; ar rcs libxxx.a *.o
        使用：gcc *.c -L /.... -l xxx -o target
    b. 共享库：用于模块的共享，利于维护。加载时链接
        创建：gcc *.c -fPIC -shared -o libxxx.so; gcc *.c -o target -L ... -lxxx
        加载：LD_LIBRARY_PATH

5 存储空间动态分配(堆上)
    a. void *malloc(size_t size); The memory is not initialized.
    b. void *calloc(size_t nmemb, size_t size); 分配nmemb个size大小的元素的空间，Initialized to zero
    c  void *realloc(void *ptr, size_t size);   给ptr重新分配size个大小的空间
    d. void free(void *ptr);

6 环境变量
    a. 字符串形式：name=value
    b. char *getenv(const char *name);
    c. int putenv(char *str);  str= "name=val"
        int setenv(const char *name, const char *value, int rewrite);
        int unsetenv(const char *name);
    d. setenv的环境表操作
        限制：环境表占用进程地址空间顶部，位于栈底之上（高地址）。不能上下扩展
        操作：
            （1）修改name：value长度<=原有value长度，可直接写入，value长度 > 原长度，
                重新malloc，并调整环境表相应指针指向
             （2）添加新的name: 为"name=value"字符串malloc空间，第一次添加时也需要为新的指针表
                malloc,此时，环境表将放在队中，表中的大多数指针还是指向栈底上的原来字符串空间.不是第一次，
                需要realloc

7 setjmp 和 longjmp 函数
    a. 函数的栈帧，ESP，EBP
    b. int setjmp(jmp_buf env);
        void longjmp(jmp_buf env, int val);

    c. 局部变量回滚问题：
        不进行优化编译：均不会回滚，为longjmp时的值
        优化编译：autoval , register value回滚，static ，volatile 不会回滚

    d. 自动变量潜在问题(demo_autoret)

8 getrlimit 和 setrlimit
    a. 
        int getrlimit(int resource, struct rlimit *rlim);
        int setrlimit(int resource, const struct rlimit *rlim);
        @resource:<resource.h>
        @struct rlimit{
            unsigned long rlim_cur;
            unsigned long rlim_max;
        };
#endif

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

static jmp_buf env;
static void demo_env()
{
    puts("enviroment:");
    printf("%s: %s \r\n", "HOME", getenv("HOME"));
    printf("%s: %s \r\n", "LANG", getenv("LANG"));
    printf("%s: %s \r\n", "PATH", getenv("PATH"));
}

static void demo_jmp2()
{
    longjmp(env, 2);
}

static void demo_jmp1()
{
    demo_jmp2();
}

static void demo_maxfileopen()
{
    struct rlimit file_limit;
   if( getrlimit(RLIMIT_NOFILE, &file_limit) == 0) {
       printf("file limit : current soft limit %lu, max hard limit %lu \r\n", file_limit.rlim_cur, file_limit.rlim_max);
   }
}

/*
 *错误用法
 *  demo_autoret返回后，栈帧pop,将被其他栈帧使用
 *  解决办法：static 全局声明或alloc在heap上
 * */
static char *demo_autoret()
{
    //char data[] = "hello world";
//    static char data[] = "hello world";
    char *data = "hello world";
    return data;
}

int main()
{
    demo_env();
    int autoval = 0;
    static int statval = 0;
    register int regval = 0;
    volatile int volival = 0;
    int ret;
    
    puts(demo_autoret());
    demo_maxfileopen();


    if((ret = setjmp(env)) != 0) {
        printf("err return,ret = %d\r\n", ret);
        printf("autovavl is %d \r\n", autoval);
        printf("statval is %d \r\n", statval);
        printf("regval is %d \r\n", regval);
        printf("volival is %d \r\n", volival);
        exit(-1);
    }
    autoval ++,statval++,regval++,volival++;
    demo_jmp1();
    exit(0);
}
