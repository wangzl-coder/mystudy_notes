#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "proto.h"

int main(void)
{
    struct msg_st msgbuf;
    int msgid;
    key_t key;
    key = ftok(KEYPATH, KEYPROJ);
    if(key < 0) {
        perror("ftok() error \r\n");
        exit(-1);
    }
    msgid = msgget(key, 0);
    if(msgid < 0) {
        perror("msgget() error \r\n");
        exit(-1);
    }
    msgbuf.msgtype = 1;;
    msgbuf.req_pid = (long) getpid();
    sprintf(msgbuf.frame, "%s", "this is request content \r\n");
    msgsnd(msgid, &msgbuf, sizeof(msgbuf) - sizeof(long), 0);
    exit(0);
}
