#if 0
    1 取消选项：
        int pthread_setcancelstate(int state, int *oldstate);
        可选状态：PTHREAD_CANCEL_ENABLE,PTHREAD_CANCEL_DISABLE

    2 线程取消点：在pthread_cancel之后，线程会在取消点退出
        添加自己的取消点：
        void pthread_testcancel(void);

    3 上述的描述的默认的取消类型称为推迟取消，在线程到达取消点之前，并不会真正的取消。
            修改取消类型：
            int pthread_setcanceltype(int type, int *oldtype);
        可选类型：PTHREADCANCEL_DEFERRED, PTHREAD_CANCEL_ASYNCHRONOUS
#endif
