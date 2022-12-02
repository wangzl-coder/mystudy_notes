#if 0
    1 setjmp和longjmp存在的问题:
        当捕捉到一个信号时，调用信号处理程序之前会将当前信号加入到进程的信号屏蔽字中，从信号处理程序
        返回时，将其恢复。如果在信号处理程序中longjmp跳出，将无法恢复此信号的mask，导致无法响应
    
    2 int sigsetjmp(sigjmp_buf env, int savesigs);
        @savesigs: 如果位非0值，表示env中保存当前的信号屏蔽字
      
      void siglongjmp(sigjmp_buf env, int val);
        跳转到env处，并恢复env中的信号屏蔽字
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>

static jmp_buf env;
static sigjmp_buf sigenv;
static volatile bool canjmp;    /* aviod jmp error */
static void sigalrm_handler(int signum)
{
    printf("SIGALRM task ! \r\n");
    if(canjmp) {
        canjmp = 0;
        longjmp(env, 1);
    }
}

static int setlong_jmp_demo()
{
    if(signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
        fprintf(stderr, "failed to register SIGALRM \r\n");
        return -1;
    }
    canjmp = 0;
    setjmp(env);
    canjmp = 1;
    alarm(1);
    pause();
    return 0;
}

static void siglongjmp_alrm_handler(int signum)
{
    printf("SIGALRM task start! \r\n");
    if(canjmp) {
        canjmp = 0;
        siglongjmp(sigenv, 1);
    }
}

static int sigsetlong_jmp_demo()
{
    if(signal(SIGALRM, siglongjmp_alrm_handler) == SIG_ERR) {
        fprintf(stderr, "failed to register SIGALRM \r\n");
        return -1;
    }
    canjmp = 0;
    sigsetjmp(sigenv, 1);
    canjmp = 1;
    alarm(1);
    pause();
    return 0;
}

int main()
{
    int ret;
//    ret = setlong_jmp_demo();
    ret = sigsetlong_jmp_demo();
    exit(ret);
}
