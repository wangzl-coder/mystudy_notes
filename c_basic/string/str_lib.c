#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STRLEN 20

static void strcpy_test()
{
//    char *src = "hello world";
    char *src = "hello\0world";
    char *src1 = " world";
    char src2[] = "wangzl";
    char dest[STRLEN];
    strcpy(dest, src);
    strcpy(&dest[5], src1);
    puts(dest);
    strncpy(dest, src2, strlen(src2));
    puts(dest);
    strncpy(dest, src2, strlen(src2)+1);
    puts(dest);
}

static void strcmp_test()
{
    char *src[] = {"wangzl", "wangzla", "wangzlb", "wangzna", "wangzlabc"};
    int ret;
    ret = strcmp(src[1], src[0]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[0], src[1]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[1], src[2]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[2], src[1]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[2], src[3]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[3], src[2]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[0], src[4]);
    printf("ret = %d \r\n", ret);
    ret = strcmp(src[4], src[0]);
    printf("ret = %d \r\n", ret);
}

static void strcat_test()
{
    char *src1 = "hello ";
    char *src2 = "world";
    /*
    char dest[STRLEN];
    strcpy(dest, src1);
    */
    char dest[STRLEN] = {0, };
    strcat(dest, src1);
    strcat(dest, src2);
    puts(dest);
}

static void strtok_test()
{
/*  这里不能使用指针定义
    char *str = "January, February, March, April, May, June, July, August, September, October, November, December";
*/
    char str[] = "January, February, March, April, May, June, July, August, September, October, November, December";
    char *dest;
    dest = strtok(str, ", ");
    while(dest != NULL) {
        puts(dest);
        dest = strtok(NULL, ", ");
    }
}

static void memchr_test()
{
    const char *str = "hello world";
    char *ret;
    ret = memchr(str, ' ', strlen(str));
    puts(ret);
}

int main()
{
//    strcpy_test();
//    strcmp_test();
//    strcat_test();
    strtok_test();
    memchr_test();
    exit(0);
}
