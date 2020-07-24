/*************************************************************************
	> File Name: server.c
	> Author: Allen Lee 
	> Mail: trainlee1024@gmail.com
	> Created Time: Sat Jul 18 17:18:58 2020
 ************************************************************************/

#include "scommon.h"

int main() {

    //创建守护进程
	if(fork()) {                    //创建进程
        exit(0);                    //退出父进程
    }
    setsid();                       //设置会话组

    int server_listen;				//定义套接字变量
    int Port, CHPort;				//定义监听端口，客户端端口变量
    long TimeOut; 					//定义超时变量
    char ToIp[20] = {0}; 			//定义ip变量
    char tmp[20] = {0}; 			//定义临时变量
    int len;
    len = sizeof(struct sockaddr_in);
    struct sockaddr_in *clientaddr = malloc(len);   //申请一个结构体空间用来接收客户端地址端口

    memset(tmp, 0, sizeof(tmp));                    //清空
    get_conf_value(CONF, "ServerPort", tmp);        //获取监听端口
    Port = atoi(tmp);                               //字符端口转整形

    server_listen = socket_create(Port);            //创建套接字并且绑定 
    while (TRUE) {
        if (listen(server_listen, 10) < 0) {        //监听
            return -1;                              //失败返回-1
        }
        DEBUG_PRINT("1\n");
        int new_sock_fd;                            //定义一个新套接字
        if((new_sock_fd = accept(server_listen, (struct sockaddr *)clientaddr, &len)) < 0) {    //接收客户端ip，端口等信息
            writ_log_file(FALSE, "accept error!");  //写日志
            continue ;
        }
        if (new_sock_fd > 0) {
            writ_log_file(TRUE, "用户已连接");      //连接成功写日志
        }
        pthread_t p;                                //创建一个线程号变量
        PS *pth_str = (PS *)malloc(sizeof(PS));     //创建一个结构体用于进程传参
        pth_str->sockfd = new_sock_fd;              //新套接字赋值给结构体成员

        pthread_create(&p, NULL, client_request, (void *)pth_str);  //创建线程
        pthread_detach(p);                          //放飞线程
    } 

    free(clientaddr);
    return 0;
}
