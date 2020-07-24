/*************************************************************************
	> File Name: scommon.c
	> Author: Allen Lee 
	> Mail: trainlee1024@gmail.com
	> Created Time: Wed Jul 22 18:03:08 2020
 ************************************************************************/

#include "scommon.h"

//学生结构体
typedef struct student {
        long id;                        // 每个学生的学号
        char name[MAX_LEN];             // 每个学生的姓名
        float score[MAX_LEN];           // 每个学生COURSE_NUM门功课的成绩
        float sum;                      // 每个学生的总成绩
        float aver;                     // 每个学生的平均成绩
        struct student *next;
}STU;


//学生管理结构体
typedef struct student_manage {
    int stu_num;                        //学生数量
    int course_num;                     //课程数量
    char **course_name;                 //课程名
    STU *head;                          //学生头结点
} STU_M;





//从客户端传来的数据
STU_M *get_client_student(int sockfd) {
    STU_M *stu = (STU_M *)malloc(sizeof(STU_M));
    stu->stu_num = 0;
    stu->course_num = 0;
    int len = sizeof(STU_M);
    int rec_v;
    rec_v = recv(sockfd, stu, len, 0);
    if (rec_v < 0) {
        writ_log_file(FALSE, "学生管理结构体");
    }
    

    //申请课程空间
    stu->course_name = (char **)malloc(sizeof(char*) * (stu->course_num));
    for (int i = 0; i < stu->course_num; ++i) {
        stu->course_name[i] = (char *)malloc(sizeof(char) * MAX_LEN);
    }

    //课程名接收
    for (int i = 0; i < stu->course_num; ++i) {
        rec_v = recv(sockfd, stu->course_name[i], MAX_LEN, 0);
        if (rec_v < 0) {
            writ_log_file(FALSE, "接收课程名");
        }
    }

    len = sizeof(STU);
	stu = creat(stu);
    STU *node = stu->head;
    STU *tmp = node->next;


    //接收学生信息
    for (int i = 0; i < stu->stu_num; i++) {
        if (i) {
            node = node->next;
            tmp = tmp->next;
        }
        
        rec_v = recv(sockfd, node, len, 0);
        if (rec_v < 0) {
            writ_log_file(FALSE, "接收学生结点");
        }
        
        node->next = tmp;
    }
    return stu;
}

//发送学生数据
int send_stu_data(STU_M *stu, const int sockfd) {
    int send_v;

    send_v = send(sockfd, stu, sizeof(STU_M), 0); //发送学生管理结构体
    if (send_v <= 0) {
        writ_log_file(FALSE, "发送学生管理结构体");
    }
	//发送课程名
    for (int i = 0; i < stu->course_num; ++i) {
        if (send(sockfd, stu->course_name[i], MAX_LEN, 0) <= 0) {
            writ_log_file(FALSE, "发送课程名");     //写日志
        }
    }

    STU *node = stu->head;
    int len = sizeof(STU);
    //循环发送学生信息
    for (int i = 0; i < stu->stu_num; i++) {    
        if (i) {
            node = node->next;
        }
        send_v = send(sockfd, node, len, 0);        //发送学生节点
        if (send_v <= 0) {
            writ_log_file(FALSE, "发送学生节点");
        }
    }
    return 1;
}

//线程执行客户端请求函数
void *client_request(void *pth_str) {
    int ret = 0;
    STU_M *stu = init_student();                    //初始化学生结构体并读取学生数据
    const int sockfd = ((PS *)pth_str)->sockfd;     //定义一个套接字并且从结构体获取值
    
    ret = send_stu_data(stu, sockfd);               //发送学生数据给客户端
    if (ret == TRUE) {
        writ_log_file(TRUE, "发送数据至客户端完成");
    }
    end_student(stu);                               //释放学生信息

    int flag;
    while (TRUE) {                                  //循环接收客户端请求    
        ret = recv(sockfd, &flag, sizeof(flag), 0); 
        if (flag == 0) {
            break;
        } else if (flag == 1) {
            stu = get_client_student(sockfd);       //从客户端接收学生数据
            write_to_file(stu);                     //学生信息写入到文件
            end_student(stu);                       //释放学生信息
        }
    }

    close(sockfd);                                  //关闭套接字
    free(pth_str);                                  //释放线程结构体
    writ_log_file(TRUE, "用户已经断开");
    DEBUG_PRINT("用户已经断开\n");
    return (void *)NULL;
}


//获取配置文件
int get_conf_value(char *file, char *key, char *val) {
    int ret = -1;                               //定义一个返回值
    FILE *fp = NULL;                            //定义文件指针
    char *line = NULL, *substr = NULL;          //定义行指针,子字符串指针
    size_t n = 0, len = 0;                      //定义n，长度len
    ssize_t read;                               //定义一个记录返回长度的变量

    if (key == NULL) {                          //如果key为空
        writ_log_file(FALSE, "获取配置文件key错误");                      //报错
        return ret;
    }
    fp = fopen(file, "r");                      //以只读打开文件

    if (fp == NULL) {                           //打开失败
        writ_log_file(FALSE, "配置文件未能成功打开");    //报错
        return ret;
    }

    while ((read = getline(&line, &n, fp))) {   //循环读取行
        substr = strstr(line, key);             //查找key是否在
        if (substr == NULL) {                   //如果没在
            continue;                           //跳过本次循环
        } else { 
            len = strlen(key);                  //求取长度
            if (line[len] == '=') {             //Key后面是否为=
                strncpy(val, line + len + 1, (int)read - len - 2); //找到了配置赋值给val
                ret = 1;
                break;
            } else {
                continue;                       //否则跳过本次循环
            }
        }
    }
    if (substr == NULL) {                       //如果没找到 
        writ_log_file(FALSE, "配置文件未正确填写");
        ret = -1;
    }
    fclose(fp);                                 //关闭文件
    free(line);                                 //释放line
    return 0;
}

//创建套接字函数
int socket_create(int port) {
    int sockfd;                                 //定义一个套接字
    struct sockaddr_in addr;                    //定义一个地址结构体

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {   //创建套接字
        return -1;                                          //如果失败返回-1
    }
    addr.sin_family = AF_INET;                              //协议族为ipv4
    addr.sin_port = htons(port);                            //赋予端口
    addr.sin_addr.s_addr = htonl(INADDR_ANY);               //地址为所有的

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {     //绑定
        return -1;

    }

    return sockfd;                                                      //返回套接字描述符
}

//初始化学生管理
STU_M *init_student() {
    STU_M *stu = (STU_M *)malloc(sizeof(STU_M));                    //定义学生管理结构体申请空间

    FILE * fp = fopen(STUDENT_FILE, "r");                           //打开学生数据文件
    //循环读取
    char *tmp = malloc(sizeof(char *) * MAX_LEN);                   //定义一个临时字符串


    if ((fp = fopen(STUDENT_FILE, "r")) != NULL) {                  //打开成功


            //读取学生数量和课程数量
        int num;
        if (get_conf_value(STUDENT_FILE, "学生数", tmp) != -1) {    //读取学生数
            stu->stu_num = atoi(tmp);
        }
        if (get_conf_value(STUDENT_FILE, "课程数", tmp) != -1) {    //读取课程数
            stu->course_num = atoi(tmp);
        }

        fscanf(fp, "%s\n", tmp);
        fscanf(fp, "%s\n", tmp);

        //申请课程名内存
        stu->course_name = (char **)malloc(sizeof(char*) * (stu->course_num));
        for (int i = 0; i < stu->course_num; i++) {
            stu->course_name[i] = (char *)malloc(sizeof(char) * MAX_LEN);
        }

        //读取第二行信息
        int end = stu->course_num;                                  //定义一个结尾
        fscanf(fp, "%s", tmp);                                      //读入到临时字符串
        fscanf(fp, "%s", tmp);                                      //读入到临时字符串
        for(int i = 0; i < end; i++) {
            fscanf(fp, "%s", stu->course_name[i]);                  //循环读入课程名
        }
        fscanf(fp, "%s", tmp);                                      //读入到临时字符串
        fscanf(fp, "%s\n", tmp);                                    //读入到临时字符串

        //开始读取学生数据
        stu = creat(stu);                                           //创建学生链表
        STU *p = stu->head;
        //循环从文件读入数据
            for (int i = 0; i < stu->stu_num; i++) {                //循环读入
                fscanf(fp, "%ld", &(p->id));                        //读入学号
                fscanf(fp, "%s", p->name);                          //读入姓名
            //循环读入分数
                for (int j = 0; j < stu->course_num; j++) {
                        fscanf(fp, "%f", &p->score[j]);             //读入分数
                }
                fscanf(fp, "%f%f", &p->sum, &p->aver);              //读入总分和平均分

                p = p->next;                                        //p移动到下个学生结点

            }

        //writ_log_file(TRUE, "成功读取学生数据");                  //写日志
    } else {                                                        //如果不存在文件
        fp = fopen(STUDENT_FILE, "w");                              //创建文件
        stu->stu_num = 0;                                           //学生数置0
        stu->course_num = 0;                                        //课程数置0
        fprintf(fp, "学生数%d\n课程数=%d\n", 0, 0);                            //在文件中写入学生数和课程数为0
        stu->course_name = NULL;                                    //课程名置空
        stu->head = NULL;                                           //学生头结点置空
        //writ_log_file(TRUE, "没有学生数据文件，成功创建学生数据文件");  //写日志
    }

    free(tmp);
    fclose(fp);                                                     //关闭文件描述符
    return stu;
}

//创建学生链表函数
STU_M *creat(STU_M *stu) {
        STU *head;                          //定义一个头节点
        STU *p1, *p2;                       //定义两个节点
        // system("clear");
		int len = sizeof(STU);
        for (int i = 1; i < stu->stu_num + 1; i++) {   //循环开辟空间
                p1 = (STU *)malloc(len);    //申请空间
                p1->next = NULL;            //下一个节点指向空
                if (i == 1) {               //如果是第一次
                        head = p2 = p1;     
                }
                else {
                        p2->next = p1;
                        p2 = p1;
                }
        }
    	stu->head = head;                   //赋值给学生管理结构体
        return stu;
}

//获取日期时间的函数，返回一个字符串指针，用完释放
char *get_date_time() {
    char *str = malloc(sizeof(char) * 25);                      //申请一个字符串空间
    memset(str, 0, sizeof(char) * 25);                          //全置为0
    time_t timep;                                               //定义一个记录时间戳的变量
    struct tm *p;                                               //定义一个时间结构体指针
    time(&timep);                                               //获取时间戳
    p = gmtime(&timep);                                         //获得时间结构体地址

    int year = 1900 + p->tm_year;                               //获取当前年份,从1900开始，所以要加1900
    int mon = 1 + p->tm_mon;                                    //获取当前月份,范围是0-11,所以要加1
    int day = p->tm_mday;                                       //获取当前月份日数,范围是1-31
    int hour = 8 + p->tm_hour;                                  //获取当前时,这里获取西方的时间,刚好相差八个小时
    int min = p->tm_min;                                        //获取当前分
    int sec = p->tm_sec;                                        //获取当前秒

    sprintf(str, "%d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec); //格式化到字符串

    return str;

}

//日志函数
void writ_log_file(int flag, const char *str) {
    FILE *fp = fopen(LOG_FILE, "a");                    //打开日志文件
        char *d_t = get_date_time();                    //获取日期时间字符串
    char *tmpbool = (flag ? "正常" : "错误");           //判断是错误还是正确日志
    fprintf(fp, "%s\t%d\t%s\t%s\t%s\n", __FILE__, __LINE__, tmpbool, d_t, str);     //输出到日志文件
    free(d_t);                                          //释放返回的时间字符串空间
    fclose(fp);                                         //关闭文件描述符
}

//结束学生管理
void end_student(STU_M *stu) {
    STU *p, *tmp;                                           //定义一个指针和临时指针
    p = stu->head;                                          //临时指针指学生向头结点
    stu->head = NULL;                                       //学生管理结构体的头结点指向NULL，避免野指针

    //循环释放
    while(p) {                                              //指针不为空时循环
        tmp = p;                                            //临时指针指向p
        p = p->next;                                        //p指向下一个结点
        free(tmp);                                          //释放临时指针
    }

    p = NULL, tmp = NULL;                                   //p和临时指针支持NULL
    //循环释放课程名
    for (int i = 0; i < stu->course_num; i++) {             //遍历所有课程名
        free(stu->course_name[i]);                          //释放课程名内存
    }
    free(stu->course_name);                                 //释放一级指针
    writ_log_file(TRUE, "退出系统");                        //打印日志
}

//学生信息保存到文件
void  write_to_file(STU_M *stu) {
    STU *node;              //定义一个学生节点
    node = stu->head;       //学生节点等于学生头节点
    FILE *fp;

    //打开学生数据文件
    if ((fp = fopen(STUDENT_FILE, "w")) == NULL) { 
            writ_log_file(FALSE, "未能正确打开学生数据文件");               //写日志
            return;
    }
    //写入数据
    fprintf(fp, "学生数=%d\n课程数=%d\n", stu->stu_num, stu->course_num);   //写入学生数，课程数
    fprintf(fp, "%s\t%s", "学号", "姓名");                                  
    //循环写入课程名
    for (int i = 0; i < stu->course_num; i++) { 
            fprintf(fp, "\t%s", stu->course_name[i]);                       //写入课程名
    }
    fprintf(fp, "\t%s\t%s\n", "总分", "平均分");                        

    //循环写入学生数据
    while (node) {
        fprintf(fp, "%ld\t%s", node->id, node->name);                       //写入学号，姓名
        //循环写入分数
        for (int i = 0; i < stu->course_num; i++) {
            fprintf(fp, "\t%.2f", node->score[i]);                          //写入分数
        }
        fprintf(fp, "\t%.2f\t%.2f\n", node->sum, node->aver);
        node = node->next;                  
    }
    writ_log_file(TRUE, "成功写入学生数据");    //写日志
    fclose(fp);                                 //释放文件指针

}

