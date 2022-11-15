#include <stdio.h>
#include <stdlib.h>


static void func_printenv()
{
    extern char **environ;
    char **ptr;
    printf("----------------------------pid:%d---------------------------\r\n",getpid());
    for(ptr = environ; *ptr != 0; ptr ++) {
        printf("%s \r\n", *ptr);
    }
    printf("-------------------------------------------------------------\r\n",getpid());
}

int main(int argc, char *argv[])
{
    int i;
    printf("argc = %d \r\n", argc);
    for(i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    putchar('\n');
    sleep(5);
    func_printenv();
    exit(0);
}
