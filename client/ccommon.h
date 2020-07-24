/*************************************************************************
	> File Name: ccommon.h
	> Author: Allen Lee 
	> Mail: trainlee1024@gmail.com
	> Created Time: Fri Jul 24 15:05:16 2020
 ************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <sys/ioctl.h>
#include <curses.h>

#ifndef _CCOMMON_H
#define _CCOMMON_H

//debug宏
#ifdef DEBUG
#define DEBUG_PRINT(a, ...) printf(a, ##__VA_ARGS__);   //debug输出
#else
#define DEBUG_PRINT(s)
#endif

#define MAX_LEN 20
#define CONF_FILE "./client.conf"
#define TRUE    1                       // 正确宏
#define FALSE   0                       // 错误宏
#define LOG_FILE        "client.log"    // 日志文件名
#define STUDENT_FILE    "student.txt"   // 学生数据文件
#define LEN sizeof(struct student)

#define COL 1
#define ROW 2
#define MID (get_winsize(ROW))

static char *INTIP = "(正在输入->)";
static int color_flag = 1;

//学生结构体
typedef struct student STU;

//学生管理结构体
typedef struct student_manage STU_M;

int ctrl(int ,int , STU_M *);                   //控制函数
int get_winsize(int );                          //获取窗口尺寸函数
int send_stu_data(STU_M *, const int);          //发送数据给服务器
int socket_connect(char *, int);                //连接函数
int get_conf_value(char *, char *, char *);     //获取配置文件
int str_to_int(char *str);                      //字符串转换整型
int get_stu_msg(STU_M *);                       //输入学生信息
int on_id_del(STU_M *);                         //按学号删除
int on_name_del(STU_M *);                       //按姓删除
int all_del(STU_M *);                           //删除全部学生
int get_conf_value(char *f, char *, char *);    //获取学生数和课程数
int save_to_server(STU_M *, int);               //上传学生信息
char *get_date_time();                          //获取日期时间的函数
void gui();                                     //页面框架
void menu();                                    //菜单
void color_print(int x, int y, int foreground, int background, char *p, int *); //添加有颜色的输出
void over(int);                                 //让服务器断开连接
void writ_log_file(int , const char *);         //写入日志文件
void print_msg(STU_M *);                        //打印信息函数
void sum_avg_every_course(STU_M *);             //计算每门课程的总分和平均分
void sum_avg_every_student(STU_M *);            //计算每个学生的总分和平均分
void creat_student(STU_M *);                    //创建链表并录入信息
void search_by_Num(STU_M *);                    //按学号查询学生排名及其考试成绩
void search_by_Name(STU_M *);                   //按姓名查询学生排名及其考试成绩
void statistic_analysis(STU_M *);               //统计输出
void end_student(STU_M *);                      //结束学生管理
STU *new_node();                                //创建新节点
STU_M *sort_by_score(STU_M *);                  //按每个学生的总分由高到低排出名次表
STU_M *sort_by_score_reverse(STU_M *);          //按每个学生的总分由低到高排出名次表
STU_M *creat(STU_M *);                          //创建学生链表
STU_M *sort_by_num(STU_M *);                    //按学号由小到大排出成绩表
STU_M *sort_by_name(STU_M *);                   //按姓名的字典顺序排出成绩表
STU_M *get_server_student();                    //从服务器获取学生数据

#endif
