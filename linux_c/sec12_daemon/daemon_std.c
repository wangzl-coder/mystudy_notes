#if 0
    1 守护进程的惯例
        （1）若守护进程使用锁文件，那么该文件通常存储在/var/run目录中（需要超级用户权限），锁文件的名字通常是
        name.pid, 其中name是该守护进程或服务的名字
        （2）若守护进程支持配置选项，那么配置文件通常放在/etc目录中，配置文件的名字通常是name.conf 其中name是
        该守护进程或服务的名字
        （3）守护进程可用命令行启动，通常其是由系统初始化脚本之一（/etc/rc*或/etc/init.d/*）启动的，如果守护进程
        终止时，应当自动重新启动它，则我们可以在/etc/inittab中为该守护进程包括respawn记录项，这样init就将重新启动
        该守护进程
        （4）若一个守护进程有一个配置文件，那么当该守护进程启动时会读该文件，但在此之后一般就不会再查看他，若某个
        管理员更改了配置文件，那么该守护进程可能需要被停止，然后再启动，以使配置文件更改生效，为避免此种麻烦，某些
        守护进程将捕捉SIGHUP信号，当他们接收到该信号时，重新读配置文件，因为守护进程并不与终端相结合，他们或者是无
        控制终端的会话首进程，或者是孤儿进程组的成员，所以守护进程没有理由期望接收SIGHUP。于是，守护进程可以安全地
        重复使用SIGHUP
#endif

/* demo :说明一种守护进程可以重新读其配置文件的方法 */

void *thread_func(void *arg)
{
    int err, signo;
    for( ; ; ) {
        err = sigwait(&mask, &signo);
        if(err != 0) {
            syslog(LOG_ERR, "sigwait failed ");
            exit(1);
        }
        switch(signo) {
            case SIGHUP:
                syslog(LOG_INFO, "Re_reading configure file ");
                reread();
                break;
            case SIGTERM:
                syslog(LOG_INFO, "got SIGTERM, exiting");
                exit(0);
            default:
                syslog(LOG_INFO, "unexpected signal %d \n", signo);
        
        }
    }
    return 0;
}


int main()
{
    int err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;

    if((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argvv[0];
    else 
        cmd ++;

    /* become a daemon */
    daemonize(cmd);

    /* make sure only one copy of the daemon is running */
    if(already_running()) {
        fprintf(stderr, "daemon already running \r\n");
        exit(1);
    }
    
    /* restore SIGHUP default and block all signals */
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0) {
        fprintf(stderr, "sigaction failed \r\n");
        exit(1);
    }
    sigfillset(&mask);
    if((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        fprintf(stderr, "pthread_sigmask failed \r\n");
        exit(1);
    }
    
    /* create a thread to handle SIGHUP and SIGTERM */
    err = pthread_create(&tid, NULL, thread_func, 0);
    if(err != 0) {
        fprintf(stderr, "pthread_create failed \r\n");
        exit(1);
    }
    /* process with the reset of the daemon */
    exit(0);
}
