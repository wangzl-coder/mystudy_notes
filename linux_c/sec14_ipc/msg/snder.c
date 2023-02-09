#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "proto.h"


int main()
{
    key_t key;
    int msgid;
    struct msg_st msgbuf;
    if((key = ftok(KEYPATH, KEYPROJ)) < 0) {
        perror("ftok() error \r\n");
        exit(1);
    }
    if((msgid = msgget(key, 0)) < 0) {
        perror("msgget() error \r\n");
        exit(1);
    }
    msgbuf.mtype = 1;
    sprintf(msgbuf.name, "%s","wangzl");
    msgbuf.chinese = 130;
    msgbuf.math = 149;
    if(msgsnd(msgid, &msgbuf, sizeof(msgbuf) - sizeof(long), 0) < 0) {
        perror("msgsnd() error \r\n");
        exit(1);
    }
    exit(0);
}
