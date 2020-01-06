#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "myserver.h"
#include "../include/proto.h"

int main(int argc, char **argv)
{
    int ret=0;
    int sd=0;
    int val=1;
    struct sockaddr_in l_addr;
    int l_addr_len=0;
    
    sd=socket(AF_INET, SOCK_STREAM, 0);
    if(-1==sd)
    {
        perror("socket err:");
        ret -1;
    }
    ret=setsockopt(sd,SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if(-1==ret)
    {
        goto ERR_SETSOCK;
    }

    l_addr.sin_family=AF_INET;
    l_addr.sin_port=htons(SERVER_PORT);
    l_addr.sin_addr.s_addr=INADDR_ANY;;
    l_addr_len=sizeof(struct sockaddr_in);
    ret=bind(sd, (struct sockaddr *)&l_addr, l_addr_len);
    if (-1==ret)
    {
        perror("bind()");
        goto ERR_BIND;
    }
    ret=listen(sd,200);
    if (-1==ret)
    {
        perror("listen()");
        goto ERR_LISTEN;
    }
    
    


    close(sd);
    return 0;
    
    ERR_LISTEN:
    ERR_BIND:
    ERR_SETSOCK:
        close(sd);
        return ret;
}
