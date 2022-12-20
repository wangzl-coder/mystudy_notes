#if 0
    1 线程私有数据（特定数据）：存储和查询某个特定线程相关数据的一种机制。errno

    2 分配线程特定数据之前，需要创建与该数据关联的键：
        int pthread_key_create(pthread_key_t *keyp, void (*destructor)(void *));
        @keyp: 键的回填地址，创建时每个线程的数据地址为空
        @destructor: 与改键关联的析构函数，传入参数键的地址；线程正常退出时被调用（数据地址不为空）
        一般操作：在create函数中malloc内存，在析构函数中释放内存。
        线程可以为线程特定数据分配多个键，每个键都有一个析构函数与之关联。每个键关联的析构函数
        可以互不相同，所有的键也可以使用相同的析构函数，线程退出时，线程特定数据的析构函数将按照操作
        系统实现中定义的顺序被调用，析构函数可能会调用另一个函数，该函数可能会创建新的线程特定数据，并且
        把这个数据与当前的键关联起来。当所有的析构函数被调用完以后，系统还会检查是否还有非空的线程特定数据
        值与键关联，如果有的话，再次调用析构函数，这个过程将会一直重复直到线程所有的键都为空线程特定数据值。

    3 取消键与特定数据值之间的关联关系：
        int pthread_key_delete(pthread_key_t key);
        注：并不会激活与键关联的析构函数，

    4 需要确保分配的键并不会由于在初始化阶段的竞争而发生变动，下面的代码会导致两个线程都调用pthread_key_create

    void destructor(void *);
    pthread_key_t key;
    int init_done = 0;
    int thread_func(void *arg)
    {
        if(!init_done)
            init_done = 1;
        err = pthread_key_create(&key, destructor);
    }
    解决这种竞争的办法是使用pthread_once
    pthread_once_t initflag = PTHREAD_ONCE_INIT;
    int pthread_once(pthread_once_t *initflag, void (*initfn)(void));
    initflag 必须是一个非本地变量（如全局变量或静态变量）而且必须初始化为PTHREAD_ONCE_INIT
    如果每个线程都调用phtread_once,系统就能保证初始化例程initfn只被调用一次，即系统首次调用
    pthread_once时，创建键时避免冲突的一个正确方法如下：
    void destructor(void *);
    pthread_key_t key;
    pthread_once_t init_done = PTHREAD_ONCE_INIT;
    void thread_init(void)
    {
        err = pthread_key_create(&key, destructor)
    }
    int thread_func(void *arg)
    {
        pthread_once(&init_done, thread_init);
    }
    
    5 
        void *pthread_getspecific(pthread_key_t key);
        获取与键值关联的线程特定数据
        int pthread_setspecific(pthread_key_t key, const void *value);
        关联已经初始化的键值与特定数据
#endif


/*demo : 线程安全的getenv实现，给每个线程分配特定数据保存 */

pthread_key_t key;
pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_lock = PTHREAD_MUTEX_INITIALIZER;

void privdata_free(void *arg)
{
    free(arg);
}

void keyinit_func(void)
{
    pthread_key_create(&key, privdata_free);
}


char *getenv(const char *name)
{   
    char *envbuf;
    int i, len ;
    pthread_once(&init_done, keyinit_func);
    pthread_mutex_lock(&env_lock);
    envbuf = pthread_getspecific(key);
    if(envbuf == NULL) {
        envbuf = malloc(MAXSTRINGSZ);
        if(envbuf == NULL) {
            pthread_mutex_unlock(&env_lock);
            return NULL;
        }
        pthread_setspecific(key, envbuf);
    }
    len = strlen(name);
    for(i = 0; environ[i] != NULL; i++) {
        if(strncmp(environ[i], name, len) == 0 && environ[i][len] == '=') {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ - 1);
            pthread_mutex_unlock(&env_lock);
            return (envbuf);
        }
    }
    pthread_mutex_unlock(&env_lock);
    return NULL;
}

