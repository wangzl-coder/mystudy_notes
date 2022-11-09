#if 0
1 流和FILE对象
    a. 流的定向：未定向，宽定向，字节定向
        int fwide(FILE *fp, int mode);
        @return: 宽定向--> >0, 字节定向--> <0, 未定向--> == 0
        @mode: +, -, 0

2 标准输入，标准输出，标准出错
    a. 缓冲：全缓冲（磁盘文件），行缓冲（终端），不带缓冲（stderr）；
    b. fflush,flush,刷新流，flush也可丢弃终端缓冲数据。
    c. 缓冲特征：
    d. 更改缓冲类型：
        void setbuf(FILE *stream, char *buf);
        @buff: buff[BUFSIZ] :打开缓冲，具体类型无法控制
                buff==NULL : 关闭缓冲

        int setvbuf(FILE *stream, char *buf, int mode, size_t size);
        @mode: _IOFBF, _IOLBF, _IONBF
        @buf: 缓冲区
        @size: buf长度

3 打开流
    a. 三个函数
        FILE *fopen(const char *path, const char *mode);
            打开指定文件
        FILE *fdopen(int fd, const char *mode);
            通过文件描述符获取与其对应的FILE流(pipe, socket)
        FILE *freopen(const char *path, const char *mode, FILE *stream);
            在指定流上打开指定文件，将指定的文件打开为一个预定义的流（stdin,stdout, stderr）
        mode:
            "r"(只读不创建), "w"(擦除写创建), "a"(追加写创建), "r+"（可读写不创建）, "w+"(可读可写擦除创建), "a+"(追加读写创建)
        
        注意：
            (1) 截短写和创建打开并不适用于fdopen(),
            (2) 读写打开输出后面不能直接输入，需要fflush,fseek,fsetpos或rewind
            (3) 输入操作后如果没有到达文件尾端，不能直接跟输出，需要fseek, fsetpos, 或rewind
            (4) 无法说明文件访问权限位
            (5) 除非引用终端设备，否则默认全缓冲打开,使用setbuf,setvbuf修改
            (6) int flcose(FILE *fp)
            (7) exit()调用，

4 读写流
    a. 非格式化IO读写
        （1） 单字符读写
            输入：
            int getc(FILE *stream);
            可能被定义成宏
            int fgetc(FILE *stream);
            函数
            int getchar(void);
            相当于fgetc(stdin);
            
            @return : 读的char（转为int型）或EOF（到达文件尾或出错），
                出错判断：int ferror(FILE *fp); 
                文件尾判断：int feof(FILE *fp);
                @return : 条件真，返回真
                原理：每个FILE*维护两个标志，出错和文件结束标志
                    标志清除： void clearerr(FILE *fp);
            
            int ungetc(int c, FILE *fp);
            押送字符会缓冲流中，可再次读取，读取顺序与押送顺序相反

            输出：
            int putc(int c, FILE *stream);
                可能被定义成宏
            int fputc(int c, FILE *stream);
                函数
            int putchar(int c);
                相当于fputc(c, stdout);

        （2）每次一行I/O
            输入：
            char *fgets(char *s, int size, FILE *stream);
            注意：最多读取size-1个字符，以null结尾，
                    包含换行符

            char *gets(char *s);
                不建议使用

            输出：
            int fputs(const char *s, FILE *stream);
            int puts(const char *s);

5 标准IO的效率：
        

#endif
