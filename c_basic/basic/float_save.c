#include <stdio.h>
#include <math.h>

/*
 * float存储方式：最高位符号位 0 --> +， 1 --> -,
 *
 * 30-23: 指数位（阶码），将整数部分化成二进制，将小数点前移到只
 *      有一个1,小数点移动位数+127即为存储形式.
 *
 * 22-0：尾数位，小数部分乘2之后，整数部分留下，小数部分继续乘
 *      2，直到拿到整1结束（满位后舍弃造成精度丢失），小数点移
 *      位后的小数部分当尾数存储.
 *
 * 由于存在精度丢失，float 类型不能直接==判断值，需要指定最小误差
 *
 *
 * */

#define MIN_DEVIATION (1e-6)

int main()
{
#if 0
    float pi = 3.14;
    float a = 3.625;
    if(pi == 3.14) {
        printf("pi=%.2f \r\n",pi);
    }
    if(a == 3.625) {
        printf("a=%.3f \r\n",a);
    }
#endif
    float pi = 3.14;
    if(fabs(pi - 3.14) < MIN_DEVIATION) {
        printf("此时可认为pi的值为 %.2f \r\n",pi);
    }
    return 0;
}
