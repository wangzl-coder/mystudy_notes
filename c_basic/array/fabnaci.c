#include <stdio.h>
#include <stdlib.h>

/*
 * 斐波那契数列：前两项为1，后面每一项等于前两项之和
 *  这里使用指针变量实现，有助于加强指针以及指针和数
 *  组关系的理解.
 *
 *
 * */
static int fabonacci(int count, int *p)
{
    int i;
    int *q = p;
    if(count <= 2 || q == NULL)
        return -1;

    *q++ = (*q++ = 1);
    for(i = 2; i < count; i++)
    {
        *(q+2) = *(--q) + *(--q);
        q += (2 + 1);
    }

    return 0;
}

/*
 * 固定长度（数组）传参
 *
 * */
static void func1()
{
    int a[8];
    fabonacci(8, a);
    for(int i = 0; i < 8; i++) 
    {
        printf("%d ",a[i]);
    }
}


/*
 * 从终端拿长度
 *
 * */
static void func2()
{
    printf("please input the count for fab \r\n");
    while(1) {
        int count;
        int ret;
        ret = scanf("%d", &count);
        if(ret != 1) {
            while(getchar() != '\n');   //循环读入，清空输入缓冲内容
            printf("invalid, please input again \r\n");
        } else {
            int *a = malloc(count * sizeof(int));
            ret = fabonacci(count, a);
            if(ret < 0) {
                printf("The minimum number of inputs is 3 \r\n");
                free(a);
            } else {
                printf("count %d of fab is : \r\n",count);
                int *p = a;
                for(int i = 0; i < count; i++, p++)
                    printf("%d ", *p);
                putchar('\n');
                free(a);
                break;
            }
        }
    }

}

int main()
{
//    func1();
    func2();
    exit(0);
}
