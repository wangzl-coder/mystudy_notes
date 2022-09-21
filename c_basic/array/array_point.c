#include <stdio.h>
#include <stdlib.h>

/*
 * 一维数组和指针结合的常见用法
 *      数组指针：指向数组的指针变量
 *      指针数组：数组每个元素都是指针类型
 * */

/*
 *  数组指针：指向数组的指针
 *      变量类型 (*变量名)[下标] 如果没指定下标，++等操作将被报错
 *
 * */
static void array_point()
{
    int a[10] = {10,20,30,40,50,60,70,80,90,0};
    int (*arrpoint)[10];
    arrpoint = &a;  //注意是a的地址，虽然a和&a数值上相等，但是意义不同
    printf("sizeof(arrpoint) = %ld , sizeof(*arrpoint) = %ld, sizeof(**arrpoint) = %ld \r\n",
            sizeof(arrpoint), sizeof(*arrpoint), sizeof(**arrpoint)); //**arrpoint = arrpoint指向数组的首元素

    /* 三者数值相等，arrpoint->arrpoint指向的数组地址即a的地址，*arrpoint->arrpoint指向的数组的首元素的地址 */
    printf("a = %p, arrpoint = %p, *arrpoint = %p \r\n", a, arrpoint, *arrpoint);
    for(int i = 0; i < sizeof(*arrpoint)/sizeof(**arrpoint); i++) {
        /* 访问当前指向数组的元素时，考虑优先级问题需要加括号，如果不加括号，变成了访问
         *arrpoint偏移i个数组大小的首地址 */
        printf("*arrpoint[%d]=%d \r\n", i, *((*arrpoint) + i));     // *((*arrpoint) + i) = (*array_point)[i]
        
    }
    printf("%s end \r\n\n", __FUNCTION__);
}

/*
 *数组指针和二维数组
 *    数组指针可认为是二维数组的行指针
 * */
static void darr_hpoint1(int ptr[][5], int hcount)
//static void darr_hpoint1(int (*ptr)[5], int hcount)
{
    int i, j;
    printf("sizeof(ptr) = %ld \r\n", sizeof(ptr));
    printf("sizeof(ptr[0]) = %ld \r\n", sizeof(ptr[0]));
    printf("ptr = %p \r\n", ptr);
    
    for(i = 0; i < hcount; i++) {
        for(j = 0; j < 5; j++) {
            printf("%d ", (*ptr)[j]);
        }
        putchar('\n');
        ptr ++;
        printf("ptr = %p \r\n", ptr);
    }
}



int main()
{
    int a[2][5] = {
        {1,2,3,4,5},
        {6,7,8,9,0}
    };
    array_point();
    darr_hpoint1(a, sizeof(a)/sizeof(*a));
    exit(0);
}
