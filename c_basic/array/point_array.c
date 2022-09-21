#include <stdio.h>
#include <stdlib.h>




/*
 * 一维数组和指针结合的常见用法
 *      数组指针：指向数组的指针变量
 *      指针数组：数组每个元素都是指针类型
 * */

/*
 * 指针数组
 *  变量类型 *数组名[下标]
 *
 * */
static void point_array()
{
    int arr[10] = {0,1,2,3,4,5,6,7,8,9};
    int *parray[10] = {NULL};    //定义十个元素数组parray，parray每一项都是指向int类型的指针
    printf("sizeof(parray)=%ld, sizeof(*parray)=%ld \r\n",sizeof(parray), sizeof(*parray));
#if 0
    for(int i = 0; i < sizeof(parray)/sizeof(*parray); i++) {
        parray[i] = &arr[i];
    };
    for(int i = 0; i < sizeof(parray)/sizeof(*parray); i++) {
        printf("*parray[%d]=%d \r\n", i, *parray[i]);
    };
#endif
    /* 99乘法表 */
    for(int i = 0; i < 9; i++) {
        parray[i] = malloc((i+1)*sizeof(int));
        for(int j = 0; j < (i+1); j++) {
            *(parray[i] + j) = (i+1)*(j+1);
            printf("%d ",*(parray[i] + j));
        }
        putchar('\n');
        free(parray[i]);
    }
    printf("%s end! \r\n\n", __FUNCTION__);
}

/*
 * 数组传参：两种写法表现一致，都认为是指向数组某个元素地址（不一定首地址）的元素类型的指针变量
 * */
//static void array_param(int *arr)
static void array_param(int arr[10])
{
    printf("sizeof(a)=%ld \r\n", sizeof(arr));
    for(int i = 0; i < 10; i++) {
        printf("arr[%d] = %d \r\n", i, *arr);
        (*arr++)++; 
    }
    putchar('\n');
}



int main()
{
    int a[10] = {0,1,2,3,4,5,6,7,8,9};
    point_array();
    array_param(a);
    for(int i = 0; i < sizeof(a)/sizeof(*a); i++)
    {
        printf("a[%d] = %d \r\n", i, a[i]);
    }
    exit(0);
}
