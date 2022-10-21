#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "arrlist.h"


int main()
{
    int i = 0;
    int ret;
    struct arrlist_st *arrlist = NULL;
    
    ret = array_list_create1(&arrlist, 100);
    if(ret < 0)
        exit(ret);
    
    for(i = 50; i > 0; i--)
    {
        ret =  array_list_insert(arrlist, 50 - i, i);
        if(ret < 0) {
            if(ret == -ENOSPC)
                puts("data to mach !");
            puts("insert data failed");
            break;
        }
    }

    array_list_display(arrlist);
    array_list_delete(arrlist, 60);
    putchar('\n');
    array_list_display(arrlist);
    putchar('\n');
    ret = array_list_query(arrlist, 20);
    printf("data 20 index is %d \r\n", ret);
    array_list_release(&arrlist);
    
    exit(0);
}
