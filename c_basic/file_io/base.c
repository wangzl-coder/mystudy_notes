#include <stdio.h>
#include <string.h>


int main()
{
    FILE *fp;
    int i = 0;
    long pos;
    char buff[50];
    fp = fopen("tmp", "w+");
    if(fp == NULL) {
        perror(NULL);
        return -1;
    }
    while(i < 26) {
        fputc(i+'a', fp);
        i++;
    }
    pos = ftell(fp);
    printf("%ld \r\n", pos);
    rewind(fp);
    pos = ftell(fp);
    printf("%ld \r\n", pos);
    fputs("hello\nworld", fp);
    fseek(fp, 0, SEEK_SET);
    fgets(buff, 8, fp);
    printf("strlen = %ld,%s", strlen(buff), buff);
    putchar(fgetc(fp));

    fclose(fp);
    return 0;
}
