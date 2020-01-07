#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "../include/proto.h"

// ./a.out ip
int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in remote_addr;
    char buf[sizeof(DEVICE_DATA)] = {0};
    DEVICE_DATA device;
    int cnt;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // bind()
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(SERVER_PORT);
    inet_aton(SERVER_IP, &remote_addr.sin_addr);

    if (connect(sd, (void *)&remote_addr, sizeof(remote_addr)) < 0)
    {
        perror("connect()");
        goto ERROR;
    }
    sprintf(device.device_addr, "02#-101");
    sprintf(device.device_name, "HAVC-MAU");
    device.device_id=2101;
    device.temp=24;
    device.hum=66;
    device.dp_temp=18;
    device.dip_fire=0;
    device.PM25=12;
    device.smog=200;
    cnt = write(sd, &device, sizeof(DEVICE_DATA));
    if (cnt < 0)
    {
        // EINTR
        perror("write()");
        goto ERROR;
    }
    if (cnt == 0)
        exit(0);
ERROR:
    close(sd);
    exit(1);
}
