#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    int c;
    
    puts("================================= this is pull1 =================================\r\n");
    while((c = getchar()) != EOF) {
        if(putchar(c) != c) {
            perror("put error");
        }
    }
    if(ferror(stdin)) {
        perror("read error");
    }
    puts("================================= pull1 end =================================\r\n");
    exit(0);
}
