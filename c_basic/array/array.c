#include <stdio.h>
#include <stdlib.h>


/*
 * 数组的指针引用
 * 数组名：数组首地址常量（所以避免出现 char str[10]; str = "hello";等常量赋值的错误做法）
 * 指针变量：保存地址值的变量
 *
 * */
void func1()
{
    struct my_st{
        int a;
        char c;
    };

    struct my_st st[5];
    struct my_st *pst = st;
    printf("sizeof(*st) = %lx \r\n", sizeof(*st));
    printf("st = %p \r\n", st);
    printf("pst = %p \r\n", pst);
    for(int i = 0; i < sizeof(st)/sizeof(*st); i++) {
        pst->a = i;
        pst->c = 'a' + i;
        pst ++;
    }

    for(int i = 0; i < sizeof(st)/sizeof(*st); i++) {
        printf("st[%d].a=%d, st[%d].c=%c \r\n", i, st[i].a, i, st[i].c);
    }
    printf("&st[4] = %p \r\n", &st[4]);
    printf("pst = %p \r\n", pst);
    do {
        pst --;
        printf("pst->a = %d , pst->c = %c \r\n",pst->a, pst->c);
    }while(pst != st);
    printf("%s End \r\n\n", __FUNCTION__);
}

static void func2()
{
    int *p = (int[5]){1,2,3,4,5};
    for(int i = 0; i < 5; i++) {
        printf("%d \r\n", *p++);
    }
    printf("++*(--p) = %d \r\n", ++*(--p));

}



int main()
{
    func1();
    func2();
    exit(0);
}
