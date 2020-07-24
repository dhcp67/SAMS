/*************************************************************************
	> File Name: client.c
	> Author: Allen Lee 
	> Mail: trainlee1024@gmail.com
	> Created Time: Sun Jul 19 18:27:18 2020
 ************************************************************************/

#include "ccommon.h"

int main() {
    int con = TRUE;     //con用来做循环判断
    int sockfd; //定义一个套接字变量
    char *ip = (char *)malloc(sizeof(char) * MAX_LEN); //定义一个ip并申请空间
    char *port = (char *)malloc(sizeof(char) * MAX_LEN); //定义一个ip并申请空间

	get_conf_value(CONF_FILE, "To", ip);    //获取ip地址
    get_conf_value(CONF_FILE, "ServerPort", port);//获取端口
    sockfd = socket_connect(ip, atoi(port)); 
    if (sockfd < 0) {
        writ_log_file(FALSE, "连接失败"); //写日志
        endwin();
        printf("连接失败\n");
        exit(-1);
    }
    writ_log_file(TRUE, "已经连接上服务器"); //写日志

    setlocale(LC_ALL,"");                //设置编码
    WINDOW *wind = initscr();            //初始化一个窗口

    STU_M *stu;
    stu = get_server_student(sockfd);                            //从服务器获取学生数据

    int flag;
    char *in = (char *)malloc(sizeof(char) * MAX_LEN);    //定义一个输入判断标志并并申请空间
    //循环操作
    while(con) {
        gui();                                          //创建框架
        menu();                                         //创建菜单
        if (con < 0) {
            color_print(19, 3, COLOR_RED, COLOR_BLACK, "输入错误，请重新输入", &color_flag); //有颜色输出
        }
        color_print(18, 3, COLOR_BLUE, COLOR_BLACK, INTIP, &color_flag); //有颜色输出
        refresh();                                      //刷新窗口
        move(18, 17);                                   //移动
        getstr(in);                                     //输入
        //flag = atoi(in);                          //输入转整型   
        flag = str_to_int(in);                          //输入转整型   
        con = ctrl(flag, sockfd, stu);                  //调用操作函数
        erase();                                        //清屏
    }

    free(in);//释放判断标志
    end_student(stu);//

    free(ip);
    free(port);
    endwin();         //结束窗口
    printf("已退出!\n");
    return 0;
}

