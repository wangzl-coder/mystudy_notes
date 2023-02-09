#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "queue_pool.h"


#define TASKNUM (3)
#define QUEUELEN (20)

void *num_print_task(void *arg)
{
    int *data;
    queue_pool_t *qpool = arg;
    while(1) {
        data = queue_pool_pull(qpool);
        if(data == NULL)
            break;
        printf("%ld - %d \r\n", pthread_self(), *data);
        usleep(200000);
    }
    return (void *)0;
}

int main()
{
    int i;
    pthread_t tid[TASKNUM];
    int data[50];
    int *ret;
    queue_pool_t *qpool;
    qpool = queue_pool_init(QUEUELEN);
    for(i = 0; i < TASKNUM; i++) {
        pthread_create(&tid[i], NULL, num_print_task, qpool);
    }
    for(i = 0; i < 50; i++) {
        data[i] = i;
        queue_pool_push(qpool, &data[i]);
    }
    queue_pool_complete(qpool);
    for(i = 0; i < TASKNUM; i++) {
        pthread_join(tid[i], (void **)&ret);
        printf("task %d exit status %ld \r\n", i, (long int)ret);
    }
    queue_pool_destroy(qpool);
    exit(0);
}
