#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "arrlist.h"
#include <unistd.h>


int arrlist_base_test()
{
    int i = 0;
    int ret;
    struct arrlist_st *arrlist = NULL;

    ret = array_list_create1(&arrlist, 100);
    if(ret < 0)
        return ret;

    for(i = 50; i > 0; i--) {
        ret = array_list_insert(arrlist, 50 - i, i);
        if(ret < 0) {
            if(ret == -ENOSPC)
                puts("data to mach !");
            puts("insert data failed");
            break;
        }
    }

    array_list_display(arrlist);
    array_list_delete(arrlist, 1);
    putchar('\n');
    array_list_display(arrlist);
    putchar('\n');
    ret = array_list_query(arrlist, 20);
    printf("data 20 index is %d \r\n", ret);
    if(!(ret = array_list_isempty(arrlist))) {
        puts("arrlist is not empty, set it");
        ret = array_list_set_empty(arrlist);
    }
    if(array_list_isempty(arrlist)) {
        puts("arrlist is empty");
    }
    for(i = 0; i < 20; i++) {
        ret = array_list_insert(arrlist, i, i + 1);
        if(ret < 0) {
            if(ret == -ENOSPC)
                puts("data to mach !");
            puts("insert data failed");
            break;
        }
    }
    
    array_list_display(arrlist);
    array_list_release(&arrlist);
    
    return ret;
}

int arrlist_combine_test()
{
    int i = 0;
    int ret;
    struct arrlist_st *src_list = NULL;
    struct arrlist_st *dest_list = NULL;

    ret = array_list_create1(&src_list, 20);
    if(ret < 0)
        return ret;

    dest_list = array_list_create(40);
    if(dest_list == NULL) {
        ret = -1;
        goto src_release;
    }
    for(i = 0; i < 20; i++) {
        ret = array_list_insert(src_list, i, i + 1);
        if(!ret) {
            ret = array_list_insert(dest_list, i, 20 - i);
        }
        if(ret < 0) {
            if(ret == -ENOSPC)
                puts("data to mach !");
            puts("insert data failed");
            break;
        }
    }
    puts("src_list: ");
    array_list_display(src_list);
    putchar('\n');
    puts("dest_list: ");
    array_list_display(dest_list);
    ret = array_list_combine(dest_list, src_list);
    putchar('\n');
    if(ret < 0) {
        puts("combine filed !");
        goto all_release;
    }
    puts("combine_list: ");
    array_list_display(dest_list);

all_release:
    array_list_release(&dest_list);
src_release:
    array_list_release(&src_list);
    return ret;
}

int main()
{
    int ret;
//    ret = arrlist_base_test();
    ret = arrlist_combine_test();
    putchar('\n');
    exit(ret);
}
