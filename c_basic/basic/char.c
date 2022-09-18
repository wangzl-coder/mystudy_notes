#include <stdio.h>
#include <stdlib.h>
/*
 * 常见的ASCII码值：
 * NULL('\0') -> 0x00, 
 * '0' -> 0x30, '0'-'9' -> (n-0)+'0'
 * 'A' -> 0x41, 'A'-'Z' -> (n-A)+'A'
 * 'a' -> 0x61, 'a'-'z' -> (n-a)+'a'
 *
 *
 * 转义字符：
 * '\ddd' 三位八进制数
 * '\xff' 两位十六进制
 *
 * */

int main(void)
{
    int i;
    char char_num = 0x30;
    char upper_word = 0x41;
    char lower_word = 0x61;
    for(i = 0; i <= 9; i++) {
        putchar(char_num);
        putchar(' ');
        char_num ++;
    }
    putchar('\n');
    for(i = 0; i < 26; i++) {
        putchar(upper_word);
        putchar(' ');
        upper_word ++;
    }
    putchar('\n');
    for(i = 0; i < 26; i++) {
        putchar(lower_word);
        putchar(' ');
        lower_word ++;
    }
    putchar('\n');
    printf("%s -- > %o = %d \r\n","'\\015'", '\015', '\015');
    //printf("%s -- > %d \r\n","'\\018'", '\018');  ！错误用法
    printf("%s -- > %#x = %d \r\n", "'\\x5f'", '\x5f', '\x5f');

    exit(0);
}
