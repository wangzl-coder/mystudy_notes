#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int i ;
    puts("args : \r\n");
    for(i = 0 ; i < argc; i++) {
        printf("%s \r\n", argv[i]);
    }
    exit(0);
}
