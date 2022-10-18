#include <stdio.h>

//static void double_array_param(int a[][3], int row)
static void double_array_param(int (*a)[3], int row)
{
    int i, j;
    printf("%s: sizeof(a[][3])=%ld \r\n", __FUNCTION__, sizeof(a));
    for(i = 0; i < row; i++) {
        for(j = 0; j < 3; j++) {
            a[i][j] = i + j;
        }
    }
    printf("%s:   a=%p \r\n", __FUNCTION__, a);
    printf("%s: ++a=%p \r\n", __FUNCTION__, ++a);
}

static void double_array_dec()
{
//    int a[2][3] = {{1,2,3}, {4,5,6}};
//    int a[2][3] = {{1,2}, {4,}};
//    int a[2][3] = {{1,2}};
//    int a[2][3] = {1,2,3,4,5,6};
//    int a[2][3] = {1,2,3,4,5};
//    int a[][3] = {{1,2,3}, {4,5,6}};
    int a[][3] = {1,2,3,4,5,6};
    int (*b)[3] = a;

    int i, j;
    printf("sizeof(a)=%ld \r\n", sizeof(a));
    printf("sizeof(b)=%ld \r\n", sizeof(b));
    for(i = 0; i < 2; i++) {
        for(j = 0; j < 3; j++) {
            printf("%p-->%d ", &a[i][j], a[i][j]);
            printf("%p-->%d ", &b[i][j], b[i][j]);
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
    double_array_param(a, 2);
    for(i = 0; i < 2; i++) {
        for(j = 0; j < 3; j++) {
            printf("%p-->%d ", &a[i][j], a[i][j]);
            printf("%p-->%d ", &b[i][j], b[i][j]);
            putchar(' ');
        }
        putchar('\n');
    }
}

int main()
{
    double_array_dec();
    return 0;
}
