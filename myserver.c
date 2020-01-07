
#include "myserver.h"
#include "../myinclude/proto.h"

int main(int argc, char **argv)
{
    int ret = 0;
    int sd = 0;
    int c_sd = 0;
    int recv_len = 0;
    int flag = 0;
    char rcv_buf[sizeof(DEVICE_DATA)] = {0};
    DEVICE_DATA device;
    int val = 1;
    struct sockaddr_in l_addr;
    int l_addr_len = 0;
    pthread_t pid;

    //创建套接字
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sd)
    {
        perror("socket err:");
        ret - 1;
    }
    //设置套接字选项，端口可重复绑定
    ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (-1 == ret)
    {
        goto ERR_SETSOCK;
    }

    l_addr.sin_family = AF_INET;
    l_addr.sin_port = htons(SERVER_PORT);
    l_addr.sin_addr.s_addr = INADDR_ANY;
    
    l_addr_len = sizeof(struct sockaddr_in);
    //套接字绑定IP、端口号
    ret = bind(sd, (struct sockaddr *)&l_addr, l_addr_len);
    if (-1 == ret)
    {
        perror("bind()");
        goto ERR_BIND;
    }
    //监听socket
    ret = listen(sd, 200);
    if (-1 == ret)
    {
        perror("listen()");
        goto ERR_LISTEN;
    }
   /* c_sd = accept(sd, NULL, NULL);
    if (c_sd == -1)
    {
        printf("accept err\r\n");
    }*/

    while (1)
    {
        c_sd = accept(sd, NULL, NULL);
        if (c_sd == -1)
        {
            printf("accept err\r\n");
            continue;
        }
        pthread_create(&pid, NULL, (void *)pthread_handle,(void *)&c_sd);
    }

    flag = fcntl(c_sd, F_GETFL, 0);
    fcntl(c_sd, F_SETFL, flag & ~O_NONBLOCK);                         //设置成阻塞模式；
    recv_len = recv(c_sd, rcv_buf, sizeof(DEVICE_DATA), MSG_WAITALL); //客户端 socket 关闭后， recv不再阻塞 返回-1。
    if (recv_len < 0)
    {
        fprintf(stderr, "recv error %s errno: %d\n", strerror(errno), errno);
        //continue;
    }
    memcpy(&device, rcv_buf, sizeof(DEVICE_DATA));
    printf("device: name-%s, addr-%s, id-%d, temp-%d, hum-%d, dp_temp-%d, dip_fire-%d, smog-%d, PM25-%d\r\n",
           device.device_name, device.device_addr, device.device_id, device.temp, device.hum, device.dp_temp,
           device.dip_fire, device.smog, device.PM25);
    close(c_sd);

    close(sd);
    return 0;

ERR_LISTEN:
ERR_BIND:
ERR_SETSOCK:
    close(sd);
    return ret;
}

void *pthread_handle(void *c_sd)
{
    int sd=*((int *)c_sd);
    int recv_len = 0;
    int flag = 0;
    char rcv_buf[sizeof(DEVICE_DATA)] = {0};
    DEVICE_DATA device;

    flag = fcntl(sd, F_GETFL, 0);
    fcntl(sd, F_SETFL, flag & ~O_NONBLOCK);                         //设置成阻塞模式；
    recv_len = recv(sd, rcv_buf, sizeof(DEVICE_DATA), MSG_WAITALL); //客户端 socket 关闭后， recv不再阻塞 返回-1。
    if (recv_len < 0)
    {
        fprintf(stderr, "recv error %s errno: %d\n", strerror(errno), errno);
    }
    memcpy(&device, rcv_buf, sizeof(DEVICE_DATA));
    printf("device: name-%s, addr-%s, id-%d, temp-%d, hum-%d, dp_temp-%d, dip_fire-%d, smog-%d, PM25-%d\r\n",
           device.device_name, device.device_addr, device.device_id, device.temp, device.hum, device.dp_temp,
           device.dip_fire, device.smog, device.PM25);
    close(sd);

    return NULL;
}