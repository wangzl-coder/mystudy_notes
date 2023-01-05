#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    int c;
    
    while((c = getchar()) != EOF) {
        if(putchar(c) != c) {
            perror("put error");
        }
    }
    if(ferror(stdin)) {
        perror("read error");
    }
    exit(0);
}
