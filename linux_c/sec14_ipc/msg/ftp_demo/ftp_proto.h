#ifndef FTP_PROTO_H__
#define FTP_PROTO_H__

#define FTP_KEYPATH "./ftp_keyfile"
#define FTP_KEYPROJ 'F'

#define FPATHLEN (200)
#define FCONTMAX (1024)

enum ftp_msgtype_e{
    FTP_MSG_PATH = 0x01,
    FTP_MSG_CONT,
    FTP_MSG_EOF,
};

struct req_path_st{
    long msgtype;
    char fpath[FPATHLEN];
};

struct file_cont_st{
    long msgtype;
    off_t flen;
    char fcont[FCONTMAX];
};

typedef union ftp_msg_un{
    long msgtype;
    struct req_path_st pathreq;
    struct file_cont_st respcont;
}ftp_msg_t;

#endif
