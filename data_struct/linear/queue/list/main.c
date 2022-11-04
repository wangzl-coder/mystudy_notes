#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


int main()
{
    int a[] = {0,1,2,3,4,5,6,7,8,9};
    int i;
    list_queue *queue = queue_init();
    for(i = 0; i < sizeof(a)/sizeof(*a); i++) {
        queue_send(queue, &a[i]);
    }

    for(i = 0; i < sizeof(a)/sizeof(*a) - 1; i++) {
        int *b = NULL;
        if(queue_is_empty(queue)) {
            fprintf(stderr, "queue is empty \r\n");
            break;
        }
        b = queue_receive(queue);
        if(b != NULL)
            printf("b : %d \r\n", *b);
    }
    queue_release(queue);
    exit(0);
}
