#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 空白符（换行符，回车符，制表符，空格，换页符）占位,作用等同
 *
 * */
static void func1()
{
    int i, j;
    puts("input for int");
    scanf("%d %d",&i,&j);   //输入一个数之后回车，会输入第二次，
    printf("input i = %d j = %d \r\n", i, j);

    puts("input for int");
    scanf("%d\n",&i);   //输入回车之后，会跳过空白符匹配第二个输入，故需要输入两次，i匹配第一个输入字符
    printf("input i = %d \r\n", i);
}

/*
 * 缓冲区问题
 * 现象：
 *      输入 a 回车(\n) b 回车(\n) 循环结束
 *      此时 buff = {'a', '\n', 'b', '\n', '\0'}
 * 原因：
 *      输入a回车之后，'a'和'\n'留在缓冲区，buff[0]匹配到'a', buff[1]匹配到'\n'
 *      输入b回车车之后，'b'和'\n'留在缓冲区，buff[2]匹配到'b', buff[3]匹配到'\n'
 *
 * 解决办法：
 *      (1)匹配一次之后刷新stdin流缓冲，并丢弃内容，编译器可能认为是错误用法，不常用
 *     （2）检测到\n跳过重新scanf
 *
 * */
static void func2()
{   
    char buff[5] = {0};
    puts("input for 4 chars");
/*
    for(int i = 0; i < 4; i++) {
        scanf("%c",&buff[i]);
    }
*/
    for(int i = 0 ; i < 4; i++) {
        do {
            scanf("%c", &buff[i]);
        }while(buff[i] == '\n');
    }
    puts(buff);
}

/*
 * 匹配问题
 * 现象:
 *      输入a回车，第二个scanf无效，打印a = 100, c = c, d = a
 *  原因：
 *      scanf等待输入缓冲区有数据，有数据后按顺序匹配字符，匹配成功继续下一个匹配，
 *      匹配不成功，后面都不再匹配，数据仍在缓冲区，这里输入字符型'a'与%d不匹配，
 *      前两个scanf均跳过，d匹配成功拿到a
 *
 *  解决办法：
 *      尽量使用%s匹配后再进行转换
 *
 * */
static void func3()
{
    int a = 100;
    char c = 'c';
    char d = 'd';
    scanf("%d%c", &a, &c);
    scanf("%d%c", &a, &c);
    scanf("%c", &d);
    printf("a = %d, c = %c, d = %c \r\n", a, c, d);
}

/*
 * 字符串问题
 * 现象：
 *      输入hello word回车，打印hello
 *      输入20个字符，出现段错误
 * 原因：
 *      scanf匹配字符串会自动加上\0
 *      scanf 默认遇到空格结束匹配
 *      scanf不校验接收数组大小
 * 解决办法：
 *      添加修饰符[^'\n']说明只到换行符结束
 *      最好使用fgets,限定最大接收字符数，算\0(fgets会在接收字符最后自动加上尾零)
 * */
static void func4()
{
    char buff[20];
    memset(buff, 'a',sizeof(buff));
#if 0
    scanf("%s",buff);
#endif
    //scanf("%[^'\n']s", buff);
    fgets(buff, sizeof(buff), stdin);
    printf("strlen of buff is %ld \r\n",strlen(buff));
    puts(buff);
}

int main()
{
//  func1();
//  func2();
//  func3();
    func4();
    exit(0);
}
