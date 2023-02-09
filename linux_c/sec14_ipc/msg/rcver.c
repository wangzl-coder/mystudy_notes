#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "proto.h"


int main()
{
    struct msg_st msgbuf;
    int msgid;
    key_t key;
    key = ftok(KEYPATH, KEYPROJ);
    if(key < 0) {
        perror("ftok() error ! \r\n");
        exit(-1);
    }
    msgid = msgget(key, IPC_CREAT|0600);
    if(msgid < 0) {
        perror("msget() error ! \r\n");
        exit(1);
    }
    while(1) {
        if(msgrcv(msgid, &msgbuf, sizeof(msgbuf) - sizeof(long), 0, 0) < 0) {
            perror("msgrcv()");
            exit(1);
        }
        printf("name %s, chinese %d, math %d \r\n", msgbuf.name, msgbuf.chinese, msgbuf.math);
    }
    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}
