#ifndef MSG_PROTO_H__
#define MSG_PROTO_H__
 

#define KEYPATH "/home/wangzl/workspace/study/mystudy_notes/linux_c/sec14_ipc/msg/msgdemo/msgkey_file"
#define KEYPROJ 'A'

#define FRAMELEN (100)

struct msg_st{
    long msgtype;
    long req_pid;
    char frame[FRAMELEN];
};

#endif
