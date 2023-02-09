#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH "/etc/services"
#define KEYPROJ 'g'

#define NAMESIZE (20)

struct msg_st{
    long mtype;
    char name[NAMESIZE];
    int chinese;
    int math;
};

#endif
