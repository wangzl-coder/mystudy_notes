#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * O(n) - O(n^2)
 * */
static int simple_bubble_sort(int *a, int count)
{
    int i,j;

    if(!a || count <= 0) {
        return -1;
    }
    for(i = 0; i < count - 1; i++) {            //共需count - 1轮，最后剩余一个无需比较
        bool b_exchange = false;
        for(j = 0; j < count - 1 - i; j++) {    //每轮少比一个数
            if(a[j] > a[j+1]) {                 //找到相邻左大值互换位置
                int tmp = a[j];
                a[j] = a[j+1];
                a[j+1] = tmp;
                b_exchange = true;
            }
        }
        if(!b_exchange)                         //某一轮没有互换，已经有序，退出                   
            break;
    }
    return 0;
}

/*
 * O(n^2)
 * */
static int simple_select_sort(int *a, int count)
{
    int i,j,min;
    if(!a || count <= 0)
        return -1;
    for(i = 0; i < count - 1; i++) {    //总共比较count - 1轮(最后剩余一个数不用比较)
        min = i;    
        for(j = i + 1; j < count; j++) {    //每轮比较count-（i + 1)次(自身不用比较)，找最小值索引
            if(a[min] > a[j])
                min = j;
        }
        if(min != i) {                      //找到最下值索引，最小值放到左边
            int tmp = a[i];
            a[i] = a[min];
            a[min] = tmp;
        }
    }
    return 0;
}

/*
 * O(n*log(n))
 * */
static int simple_quick_sort(int *a, int left, int right)
{
    if(!a || left < 0 || right < 0)
        return -1;

    if(left < right) {                      //递归出口，左右相等（只剩一个元素）
        int i = left, j = right;
        int cmpval = a[left];               //指定参考值
        while(i < j) {
            while(i < j && cmpval <= a[j])  //从右向左寻找小值
                j--;
            if(i < j && cmpval > a[j])      //找到小值，放到左边
                a[i++] = a[j];

            while(i < j && cmpval > a[i])   //从左向右寻找大（相等）值，
                i++;
            if(i < j && cmpval <= a[i])     //找到大（相等）值，放到右边
                a[j--] = a[i];
        }
        if(i == j)                          //找完一轮，填充参考值
            a[i] = cmpval;
        simple_quick_sort(a, left, i - 1);  //递归左部分
        simple_quick_sort(a, i + 1, right); //递归右部分
    }
    return 0;
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
    int i, count;
    char buff[100];
    int numArr[51];
    fgets(buff, sizeof(buff), stdin);
    count =  string_to_intArr(numArr, buff);
    if(count < 0) {
        puts("invaild value \r\n");
        return -1;
    }
//    simple_bubble_sort(numArr, count);
    simple_select_sort(numArr, count);
//    simple_quick_sort(numArr, 0, count - 1);
    for(i = 0; i < count; i++) {
        printf("%d ", numArr[i]);
    }
    putchar('\n');
    exit(0);
}
