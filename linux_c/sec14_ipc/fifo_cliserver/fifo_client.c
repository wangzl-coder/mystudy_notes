#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "server_daemon.h"


enum clistate_e{
    CSTATE_CON = 0x0,
    CSTATE_REU,
    CSTATE_WRESP,
    CSTATE_DISCON,
    CSTATE_QUIT
};
struct client_t {
    int sfd;
    int cfd;
    pid_t pid;
    int state;
    char respfpath[RESPBUFSIZE];
    char resp[RESPBUFSIZE];
};

struct client_t *client_init(const char *sfifopath)
{
    struct client_t *fclient;

    fclient = malloc(sizeof(struct client_t));
    if(fclient == NULL)
        return(NULL);
    if((fclient->sfd = open(sfifopath,  O_WRONLY)) < 0) {
        fprintf(stderr, "open %s failed : %s \r\n", sfifopath, strerror(errno));
        free(fclient);
        return(NULL);
    }
    fclient->pid = getpid();
    fclient->cfd = -1;
    PATH_BYPID(fclient->pid, fclient->respfpath);
    return fclient;
};

static int client_send_server(struct client_t *fclient, char *buf)
{
    char pidstr[MSG_PIDBUF_LEN], *msg, msglenstr[MSGLENDESC_BUF_LEN];
    int msglen;
    char *mptr;

    sprintf(pidstr, "%ld\n", (long)fclient->pid);
    sprintf(msglenstr, "%ld\n", (long)strlen(buf));
    msglen = strlen(MSG_HEAD) + strlen(MSG_TAIL) + MSG_PIDBUF_LEN + MSGLENDESC_BUF_LEN;
    msg = malloc(msglen);
    if(msg == NULL)
        return -ENOMEM;
    mptr = msg;
    sprintf(mptr, "%s", MSG_HEAD);
    mptr += strlen(MSG_HEAD);
    memcpy(mptr, pidstr, MSG_PIDBUF_LEN * sizeof(char));
    mptr += MSG_PIDBUF_LEN;
    memcpy(mptr, msglenstr, MSGLENDESC_BUF_LEN * sizeof(char));
    mptr += MSGLENDESC_BUF_LEN;
    sprintf(mptr, "%s%s", buf, MSG_TAIL);
    if(write(fclient->sfd, msg, msglen) != msglen) {
        fprintf(stderr, "write error \r\n");
    }
    free(msg);
    return 0;
}

static int client_wait_connect(struct client_t *fclient)
{
    if(fclient->cfd > 0) {
        printf("client has connected \r\n");
        return 0;
    }
    while(fclient->cfd < 0) {
        fclient->cfd = open(fclient->respfpath, O_RDONLY);
    }
    return 0;
}

static int client_connect_server(struct client_t *fclient)
{
    if(fclient == NULL)
        return -EINVAL;
    client_send_server(fclient, REQ_CONN);
    return client_wait_connect(fclient);
}

static void client_wait_response(struct client_t *fclient)
{
    read(fclient->cfd, fclient->resp, RESPBUFSIZE);
}

static int client_request_server(struct client_t *fclient, char *cont)
{
    if(fclient == NULL)
        return -EINVAL;
    client_send_server(fclient, cont);
    client_wait_response(fclient);
    return 0;
}

static int client_disconn_server(struct client_t *fclient)
{
    if(fclient == NULL)
        return -EINVAL;
    return client_send_server(fclient, REQ_DISCON);
}

static void client_destroy(struct client_t *fclient)
{
    if(!fclient)
        free(fclient->respfpath);
    free(fclient);
}

int main()
{
    struct client_t *fclient;
    printf("client %ld \r\n", (long)getpid());
    fclient = client_init(FIFO_SERVER);
    if(fclient == NULL) {
        perror("client init failed \r\n");
        exit(-1);
    }
    client_connect_server(fclient);
    //client_request_server(fclient, "this is test client request!");
    //printf("client get resp: --%s \r\n", fclient->resp);
    client_disconn_server(fclient);
    client_destroy(fclient);
    exit(0);
}

