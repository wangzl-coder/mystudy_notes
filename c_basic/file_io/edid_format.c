#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>


#define QUIT (9)
#define TEXT_ROW_BYTES (54)
#define VAILD_DATA_PER_ROW (0x10)

#define DATA_ROW_COUNT (8)



static const char *dat_head[3] = {"EDID BYTES:\r\n", 
                                  "0x   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n",
                                  "    ------------------------------------------------\r\n"};

static int openfile_by_stdin(char *mode, FILE **fd)
{

    char path[100];
    int ret = 0;
    if(path == NULL || mode == NULL) {
        *fd = NULL;
        return -EINVAL;
    }

    while(1) {
        ret = scanf("%s", path);
        if(ret != 1) {
            puts("Invalid Input Param! \r\n");
        } else {
            if(!strcmp(path, "q")) {
                return -QUIT;
            }
            *fd = fopen(path, mode);
            if(*fd == NULL) {
                printf("open file %s failed: ", path);
                fflush(stdout);
                perror(NULL);
            } else
                break;
        }
        while(getchar() != '\n');   //clean the buffer of stdin
        puts("Try again Or q to Quit. ");
        puts("*********************** ");
    }
    return 0;
}

static int edid_dat_headcheck(FILE *fd)
{
    int i;
    char buff[1024];
    if(!fd)
        return -1;

    rewind(fd);
    for(i = 0; i < sizeof(dat_head)/sizeof(*dat_head); i++) {
        fgets(buff, sizeof(buff), fd);
        if(strcmp(dat_head[i], buff)) {
            return -1;
        }
    }
    printf("edid file check success \r\n");
    return 0;
} 

static int string_to_val(char **p, int bytes)
{
    int i;
    int value = 0;
    for(i = 0; i < bytes; i++) {
        if(**p >= '0' && **p <= '9') {
            value = ((value<<4) | (**p - '0'));
        } else if (**p >= 'A' && **p <= 'F') {
            value = ((value<<4) | (**p - 'A' +10));
        } else 
            break;
        (*p)++;
    }
    if(i != bytes)
        value = -1;

    return value;
}

static int ediddat_row_split_check(char **p)
{
    if(*((*p)++) != ' ') {
        return -1;
    }
    if(*((*p)++) != '|') {
        return -1;
    }
    return 0;
}

static int edid_dat_row_getdata(char **p, int *val)
{
    int i = 0;
    int value = 0;
    int val_num = 0;
    while(**p != '\r')
    {
        if(**p == ' ') {
            (*p)++;
            value = string_to_val(p, 2);
            if(value < 0) {
                return -1;
            }
            *val = value;
            val++;
            val_num ++;
        }
    }
    return val_num;
}


static int edid_dat_row_read(int rownum,FILE *fd,int *val)
{
    long offset = 0;
    int i;
    int ret = 0;
    int rowFlag = 0;
    char buff[1024];
    char *pst;
    if(!fd)
        return -EINVAL;

    rewind(fd);
    for(i = 0; i < sizeof(dat_head)/sizeof(*dat_head); i++) {
        offset += strlen(dat_head[i]);
    }
    offset += (TEXT_ROW_BYTES*rownum);
    fseek(fd, offset, SEEK_SET);
    fgets(buff, sizeof(buff), fd);
    pst = buff;
    rowFlag = string_to_val(&pst, 2);
    if(rowFlag < 0 || rowFlag != rownum) {
        return -1;
    }
    ret = ediddat_row_split_check(&pst);
    if(ret < 0) {
        puts("Error for Row Split Check! ");
        return ret;
    }
    ret = edid_dat_row_getdata(&pst, val);
    if(ret < 0) {
        fprintf(stderr, "edid get row %d data failed \r\n", rownum);
        return ret;
    }

    return ret;
}

int main()
{
    int ret;
    FILE *fdat = NULL;
    FILE *fcdest = NULL;
    int value[100];
    int i;
    int val_num = 0;

    putchar('\n');
    puts("------------------------------------");
    puts("input the **.dat file path for edid:");
    ret = openfile_by_stdin("r", &fdat);
    if(ret < 0) {
        return ((ret == -QUIT) ? 0 : -1);
    }
    puts("------------------------------------");
    puts("input the **.c file path for output:");
    ret = openfile_by_stdin("w+", &fcdest);
    if(ret < 0) {
        ret = (ret == -QUIT) ? 0 : -1;
        goto close_fdat;
    }
    
    ret = edid_dat_headcheck(fdat);
    if(ret < 0) {
        printf("edid file check failed \r\n");
        goto close_all;
    }
    for(i = 0; i < DATA_ROW_COUNT; i++) {
        val_num = edid_dat_row_read(0, fdat, value);
        if(val_num < 0) {
            ret = -1;
            fprintf(stderr, "edid read failed \r\n");       
            break;
        }
        printf("val_num %d \r\n", val_num);
    }
    


close_all:
    fclose(fcdest);
close_fdat:
    fclose(fdat);

    return ret;
}
