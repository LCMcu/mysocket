#include "myserver.h"
#include "../myinclude/proto.h"
#include "../myinclude/mysqlite.h"
#include "../mysqlite/mysqlite.h"

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

    //线程
    while (1)
    {
        c_sd = accept(sd, NULL, NULL);
        if (c_sd == -1)
        {
            printf("accept err\r\n");
            continue;
        }
        pthread_create(&pid, NULL, (void *)pthread_handle, (void *)&c_sd);
    }

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
    int sd = *((int *)c_sd);
    int recv_len = 0;
    int flag = 0;
    char rcv_buf[sizeof(DEVICE_DATA)] = {0};
    char *ser = "ser";
    DEVICE_DATA device;
    DEVICE_DATA_BUF device_data_buf;
    sqlite3 *db;

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

    device_data_buf.device_num = 10;
   // device_data_buf.device = malloc(sizeof(DEVICE_DATA) * device_data_buf.device_num);

    open_db("./test.db", &db);
    creat_table(db, device.device_name);
    insert_data(db, device.device_name, &device);
    select_data(db, device.device_name, "temp=24", NULL);
    update(db, device.device_name, "hum=66", "PM25", "999");
    select_data(db, device.device_name, "temp=24", &device_data_buf);
    del_data(db, device.device_name, "temp=24");
    close_db(db);
    
    return NULL;
}
