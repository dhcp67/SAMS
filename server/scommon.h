/*************************************************************************
	> File Name: scommon.h
	> Author: Allen Lee 
	> Mail: trainlee1024@gmail.com
	> Created Time: Wed Jul 22 18:03:05 2020
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

//debug宏
#ifdef DEBUG
#define DEBUG_PRINT(a, ...) printf(a, ##__VA_ARGS__);
#else
#define DEBUG_PRINT(s)
#endif

#define CONF            "./server.conf"     // 配置文件
#define MAX_LEN         20                  // 字符串最大长度
#define STUDENT_FILE    "./student.txt"     // 学生数据文件
#define LOG_FILE        "./server.log"      // 日志文件

#define TRUE    1                           // 正确宏
#define FALSE   0                           // 错误宏

typedef struct student STU;                             //学生结构体
typedef struct student_manage STU_M;                    //学生管理结构体
typedef struct pthread_struct PS;                       //传信息给线程结构体

int get_conf_value(char *file, char *key, char *val);   //获取配置文件
int socket_create(int port);                            //创建套接字函数
STU_M *creat(STU_M *);                                  //创建学生链表函数
void write_to_file(STU_M *);                            //将每个学生的纪录信息写入文件
void *client_request(void *);                           //线程执行客户端请求函数
STU_M *init_student();                                  //初始化学生管理
void end_student(STU_M *);                              //结束学生管理
void writ_log_file(int , const char *);                 //写入日志文件
char *get_date_time();                                  //获取日期时间的函数
int send_stu_data(STU_M *,const int);                   //发送学生数据
STU_M *get_client_student(int);                         //从客户端传来的数据


//传信息给线程结构体
typedef struct pthread_struct{
    int sockfd;
} PS;

#endif
