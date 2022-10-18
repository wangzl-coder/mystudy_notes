#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STRLEN 7


static void string_declare()
{
//    char str[STRLEN] = "wangzl";
//    char str[STRLEN] = "wang";
    char str[STRLEN] = "wang\0zl";
    char dest[STRLEN];
    puts(str);
    printf("str[4] = %d \r\n",str[4]);
    printf("str[5] = %c \r\n",str[5]);
    printf("str[6] = %c \r\n",str[6]);
    printf("sizeof(str) = %ld \r\n", sizeof(str));
    printf("strlen(str) = %ld \r\n", strlen(str));
    strcpy(dest, str);
    puts("\r\n dest:");
    puts(dest);
    printf("dest[4] = %d \r\n",dest[4]);
    printf("dest[5] = %c \r\n",dest[5]);
    printf("dest[6] = %c \r\n",dest[6]);
    printf("sizeof(dest) = %ld \r\n", sizeof(dest));
    printf("strlen(dest) = %ld \r\n", strlen(dest));
}


int main()
{
    string_declare();
    exit(0);
}
