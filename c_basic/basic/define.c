#include <stdio.h>
#include <stdlib.h>

/*
 * define 注意点：
 *  在预编译阶段完成（-E）
 *  优先级问题，需加括号处理
 *  标识替换，注意其他问题
 *
 * */

#define SECOND_PER_YEAR (60UL*60UL*24UL*360UL)

//#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MIN(a,b) ({         \
        typeof(a) p = a;    \
        typeof(b) q = b;    \
        (p) < (q) ? (p) : (q);   \
        })

#define NAME(name,num) (name##num)  //符号连接

#define TO_STR(na) (#na)

#define MY_DEBUG(fmt,...) \
    printf("[MY_DEBUG : [%s] <%d>]:"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

#define ITEM_OFFSET(type,item)  ((int) (&(((type *)0)->item)))


void func1(void)
{
    int i = 10, j = 20;
#if 0
    int k = MIN(i++, j++);
    printf("i=%d, j=%d, k=%d \r\n", i, j, k); // i = 12, j = 21, k = 11
#endif
    int k = MIN(i++, j++);
    printf("i=%d, j=%d, k=%d \r\n", i, j, k); // i = 11, j = 21, k = 10
    return ;
}

void func2()
{
    int NAME(a,1) = 1;
    int NAME(b,2) = 2;
    printf("%s = %d, %s = %d \r\n", TO_STR(a1), a1, TO_STR(b2), b2);
}

void func3()
{
    int i = 2;
    char *buff = "hello word";
    MY_DEBUG("i = %d \r\n",i);
    MY_DEBUG("%s \r\n", buff);
}

void func4(void)
{
    struct stu_st{
        int class;
        int age;
        float height;
    };
    printf("offset of height is %d \r\n",ITEM_OFFSET(struct stu_st, height));
    printf("second per year is %ld \r\n",SECOND_PER_YEAR);
}

int main(void)
{
    func1();
    func2();
    func3();
    func4();
    exit(0);
}
