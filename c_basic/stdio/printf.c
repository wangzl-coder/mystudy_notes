#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
* 变参函数 输出格式
*   %a(%A)      浮点数、十六进制数字和p-(P-)记数法(C99)
*   %c          字符
*   %d          有符号十进制整数
*   %f          浮点数(包括float和double)
*   %e(%E)      浮点数指数输出[e-(E-)记数法]
*   %g(%G)      浮点数不显无意义的零"0"
*   %i          有符号十进制整数(与%d相同)
*   %u          无符号十进制整数
*   %o          八进制整数    e.g.     0123
*   %x(%X)      十六进制整数<?xml:namespace prefix = st1 />()   e.g.   0x1234
*   %p          指针
*   %s          字符串
*   %%          "%"
*
* [修饰符]
* # 十六进制前加0x 八进制加0
* m: 输出宽度 正数右对齐左补空格，负数左对齐右补空格
* .n: float->保留小数点后几位,多余的四舍五入  hex->规定位数,不够补零
* + 如果是正数，前加+字符
* l: 整形long型或实型double类型
*/


void print_fmt(void)
{

    int i = 0xffffffff, j = 0xFC;
    float f= 3.14159;
    long l = 123456;
    printf("i = %u(%%u) \r\n", i);
    printf("i = %d(%%d) \r\n", i);
    printf("i = %x(%%x) \r\n", i);
    printf("i = %#x(%%#x) \r\n", i);
    i = 10;
    printf("i = %o(%%o) \r\n", i);
    printf("i = %#o(%%#o) \r\n", i);
    
    printf("\r\n");
    printf("l = %ld \r\n",l);
    printf("\r\n");

    printf("j = %d(%%d) \r\n", j);
    printf("j = %+d(%%+d) \r\n", j);
    printf("j = %4d(%%4d) \r\n", j);
    printf("j = %x(%%x) \r\n", j);
    printf("j = %#x(%%#x) \r\n", j);
    printf("j = %#.4x(%%#.4x) \r\n", j);

    printf("\r\n");

    printf("f = %f(%%f) \r\n", f);
    printf("f = %.4f(%%.4f) \r\n", f);
    printf("f = %8.2f(%%8.2f) \r\n", f);
    printf("f = %-8.2f(%%-8.2f)\r\n", f);
    printf("f = %e(%%e) \r\n", f);
    printf("f = %g(%%g) \r\n", f);

    printf("\r\n");
    printf("a = %d, b = %d \r\n",30); //编译通过(警告), a = 30, b = (30 stack上方的值)
    printf("a = %d, b = %d \r\n",30, 40, 50); //编译通过（警告）,a = 30. b = 40
}

/*
 * 行缓冲，\n强制刷新缓冲区,或者缓冲区满或程序退出时自动刷新
 *
 * */
void print_buffer(void)
{
    printf("[%s]<%d>", __FUNCTION__, __LINE__);
    sleep(1);
    printf("[%s]<%d> \n", __FUNCTION__, __LINE__);
    sleep(1);
    printf("[%s]<%d> \n", __FUNCTION__, __LINE__);
}

int main(void)
{
    print_fmt();
    print_buffer();
    exit(0);
}
