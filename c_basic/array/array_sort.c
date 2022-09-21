#include <stdio.h>
#include <stdlib.h>





static int simple_bubble_sort(int *a, int count)
{
    int i,j;

    if(count <= 0) {
        return -1;
    }
    for(i = 0; i < count - 1; i++)
    {
        for(j = 0; j < count - 1; j++) {
            if(a[j] > a[j+1]) {
                int tmp = a[j];
                a[j] = a[j+1];
                a[j+1] = tmp;
            }
        }
    
    }
}



static int string_to_intArr(int *intArr, char *str)
{
    int count = 0;
    int num = 0;
    char *p = str;
    int *n = intArr;
    
    if(!n || !p)
        return -1;
    
    while(*p != '\0' && *p != '\n') {   //fgest 会读取换行符到buff
        if(*p == ' ') {
            *n++ = num;
            count ++;
            num = 0;
        } else if(*p >= '0' && *p <= '9') {
            num = (*p - '0') + num*10;
        } else {
            printf("*p = %d \r\n", *p);
            return -1;
        }
        p++;
    }
    *n = num;
    return count+1;

}

/*
 *从终端读取数据，空格分开，暂不支持首尾空格处理
 *
 * */
int main()
{
    int count;
    char buff[100];
    int numArr[51];
    fgets(buff, sizeof(buff), stdin);
    count =  string_to_intArr(numArr, buff);
    if(count < 0) {
        puts("invaild value \r\n");
        return -1;
    }
    simple_bubble_sort(numArr, count);
    for(int i = 0; i < count; i++) {
        printf("%d ", numArr[i]);
    }
    putchar('\n');
    exit(0);
}
