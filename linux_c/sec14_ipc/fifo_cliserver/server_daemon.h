#ifndef FIFO_SERVER_TEST_H__
#define FIFO_SERVER_TEST_H__

#define FIFO_SERVER "/tmp/myfifo_daemon/fifo_server"
#define SBUFMAX PIPE_BUF
#define RESPBUFSIZE (200)
#define MSG_PIDBUF_LEN (10)
#define MSGLENDESC_BUF_LEN (10)


#define MSG_HEAD "request start \n"
#define MSG_TAIL "request end \n"
#define REQ_CONN "request connect \n"
#define REQ_DISCON "request disconnect \n"




#define PATH_BYPID(pid, buf) sprintf(buf, "/tmp/myfifo_daemon/fifo_client_%ld", (long)pid)



#endif
