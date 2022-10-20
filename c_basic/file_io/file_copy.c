#include <stdio.h>
#include <string.h>
/*
 * 文件打开模式
 * r  : 只读打开，没有报错,文件流在开头位置
 * r+ : 可读可写打开，没有报错，文件流在开头位置
 * w  : 只写打开，没有创建，文件流在开头位置
 * w+ : 可读可写打开，没有创建，文件流在开头位置
 * a  : 只追加写打开，没有创建，文件流在末尾
 * a+ : 可读可追加写打开， 没有创建，读位置在开头，写位置在末尾
 *
 * */

typedef enum{
    CPY_MODE_COVER = 0x0,
    CPY_MODE_APPEND,
} cpy_mode;

static int file_copy_all(char *dest, char *src, cpy_mode mode)
{
    int ret = 0;
    FILE *fp_src, *fp_dest;
    char *op_mode;
    char buffer[1024];
    size_t size;
    if(!dest || !src)
        return -1;

    fp_src = fopen(src, "r");
   if(fp_src == NULL) {
        fprintf(stderr, "open file %s error: ", src);
        perror(NULL);
        return -1;
    }
    op_mode = (CPY_MODE_COVER == mode ? "w+" : "a");
    fp_dest = fopen(dest, op_mode);
    if(fp_dest == NULL) {
        fprintf(stderr, "create file %s failed: ", dest);
        perror(NULL);
        ret = -1;
        goto src_close;
    }
    while((size = fread(buffer, 1, sizeof(buffer), fp_src)) > 0) {
        size_t w_last = size;
        size_t w_len = 0;
        printf("size = %ld \r\n", size);
        while((w_len = fwrite(buffer, 1, w_last, fp_dest)) > 0){
            w_last -= w_len;
            printf("w_last = %ld \r\n", w_last);
            if(w_last == 0)
                break;
        }
        if(w_len < 0) {
            fprintf(stderr, "write to file %s failed", dest);
            ret = w_len;
            goto close_all;
        }
    }
    if(size < 0) {
        fprintf(stderr, "read file %s failed", src);
        ret = size;
    }

close_all:
    fclose(fp_dest);
src_close:
    fclose(fp_src);
    return ret;
}

int main()
{
    char *dest = "copy_dest.txt";
    char *src = "copy_src.txt";
    int ret;
/*
    ret = file_copy_all(dest, src, CPY_MODE_COVER);
    if(ret < 0)
        fprintf(stderr, "copy file %s to %s failed! \r\n", src, dest);
*/
    ret = file_copy_all(dest, src, CPY_MODE_APPEND);
    if(ret < 0)
        fprintf(stderr, "copy file %s to %s failed! \r\n", src, dest);


    return ret;
}
