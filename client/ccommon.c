/*************************************************************************
	> File Name: ccommon.c
	> Author: Allen Lee 
	> Mail: trainlee1024@gmail.com
	> Created Time: Fri Jul 24 15:08:25 2020
 ************************************************************************/


#include "ccommon.h"

//学生管理结构体
typedef struct student_manage {
    int stu_num;                        //学生数量
    int course_num;                     //课程数量
    char **course_name;                 //课程名
    STU *head;                          //学生头结点
} STU_M;

//学生结构体
typedef struct student {
        long id;                                // 每个学生的学号
        char name[MAX_LEN];                     // 每个学生的姓名
        float score[MAX_LEN];                   // 每个学生COURSE_NUM门功课的成绩
        float sum;                              // 每个学生的总成绩
        float aver;                             // 每个学生的平均成绩
        struct student *next;
}STU;



//菜单二维数组
static char menu_str[17][45] = {
    "                              ",
    "1.  输入记录                  ",
    "2.  输出学生信息              ",
    "3.  按学号搜索                ",
    "4.  按名字搜素                ",
    "5.  计算每个学生的总成绩和平均",
    "6.  按分数降序排              ",
    "7.  按分数升序排序            ",
    "8.  按学号升序排              ",
    "9.  按字典序排序              ",
    "10. 按学号删除                ",
    "11. 按姓名删除                ",
    "12. 删除全部学生              ",
    "13. 统计分析                  ",
    "14. 算每门课程的总分和平均    ",
    "15. 保存到服务器              ",
    "0. 退出                       ",
};


//控制函数
int ctrl(int flag, int sockfd, STU_M *stu) {
    int ret;
    ret = flag;
    switch (flag) {
        case 1: {               //录入数据
            get_stu_msg(stu);
            break;
        }
        case 2: {               //打印数据
            print_msg(stu);
            break;
        }
        case 3: {               //按学号搜索
            search_by_Num(stu);
            break;
        }
        case 4: {               //按姓名搜索
            search_by_Name(stu);
            break;
        }
        case 5: {               //计算每个同学的总分和平均分
            sum_avg_every_student(stu);
            break;
        }
        case 6: {               //按成绩降序排序
            stu = sort_by_score(stu);
            print_msg(stu);
            break;
        }
        case 7: {               //按成绩升序排序
            stu = sort_by_score_reverse(stu);
            print_msg(stu);
            break;
        }
        case 8: {               //按学号升序排序
            stu = sort_by_num(stu);
            print_msg(stu);
            break;
        }
        case 9: {               //按姓名字典序排序
            stu = sort_by_name(stu);
            print_msg(stu);
            break;
        }
        case 10: {              //按学号删除
            on_id_del(stu);
            break;
        }
        case 11: {              //按姓名删除
            on_name_del(stu);
            break;
        }
        case 12: {              //删除全部学生
            all_del(stu);
            break;
        }
        case 13: {              //统计分析
            statistic_analysis(stu);
            break;
        }
        case 14: {              //计算每门课程的总分和平均分
            sum_avg_every_course(stu);
            break;
        }
        case 15: {              //保存到服务器
            save_to_server(stu, sockfd);
            break;
        }
        case 0: {
            over(sockfd);
            ret = FALSE; 
            break;
        }
        default: {
            ret = -1;
            break;
        }
    }
    return ret;
}

//从服务器获取学生数据
STU_M *get_server_student(int sockfd) {
    STU_M *stu = (STU_M *)malloc(sizeof(STU_M));    //为学生管理结构体开辟空间
    stu->stu_num = 0;                               //学生数为赋值0
    stu->course_num = 0;                            //课程数赋值为0
    int len = sizeof(STU_M);                        //len为STU大小
    int rec_v;                                      //定义一个返回值
    rec_v = recv(sockfd, stu, len, 0);              //发送管理结构体
    if (rec_v <= 0) {                                //如果发送失败
        writ_log_file(FALSE, "接收学生管理结构");   
    }
    
    //为课程名申请空间
    stu->course_name = (char **)malloc(sizeof(char*) * (stu->course_num));  //申请一维地址空间
    for (int i = 0; i < stu->course_num; ++i) {                             //循环申请空间
        stu->course_name[i] = (char *)malloc(sizeof(char) * MAX_LEN);       //申请空间
    }

    //课程名接收
    for (int i = 0; i < stu->course_num; ++i) {                             //循环接收课程名
        rec_v = recv(sockfd, stu->course_name[i], MAX_LEN, 0);              //接收课程名
        if (rec_v < 0) {
            writ_log_file(FALSE, "接收课程名");                             //写日志
        }
    }

    len = sizeof(STU);                                                      //len为学生节点大小
	stu = creat(stu);                                                       //创建学生链表
    STU *node = stu->head;                                                  //创建一个学生节点
    STU *tmp = node->next;                                                  //创建一个临时节点


    //接收学生信息
    for (int i = 0; i < stu->stu_num; i++) {
        if (i) {
            node = node->next;                                              //节点等于下一个节点
            tmp = tmp->next;                                                //临时节点等于下一个节点
        }
        
        rec_v = recv(sockfd, node, len, 0);                                 //接收学生信息
        if (rec_v < 0) {
            writ_log_file(FALSE, "接收学生结点");
        }
        
        node->next = tmp;                                                   //学生节点下一个节点等于临时节点
    }
    return stu;
}

//上传学生信息
int save_to_server(STU_M *stu, int sockfd) {
    int ret;                                        //定义一个返回值
    int flag = 1;                                   //定义一个标志
    ret = send(sockfd, &flag, sizeof(flag), 0);     //发送标志
    if (ret <= 0) {                                 //如果返回值小于等于0，发送失败
        writ_log_file(FALSE, "发送flag失败");       //写日志
    } else {
        flag = send_stu_data(stu, sockfd);          //发送学生数据
        if (flag <= 0) {
            writ_log_file(FALSE, "上传数据失败");   //写日志
        }
    }
    if (flag > 0) {
        writ_log_file(TRUE, "成功上传数据");        //写日志
        DEBUG_PRINT("成功上传数据\n");
    }
    return ret;
}

//创建新节点
STU *new_node() {
    STU *node = (STU *)malloc(sizeof(STU));     //申请节点空间
    node->next = NULL;                          //下一个节点指向NULL
    return node;
}

//连接函数
int socket_connect(char *host, int port) {
    int sockfd;     //定义一个套接字
    struct sockaddr_in server; //定义一个地址端口结构体

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //创建一个套接字
        return -1;
    }

    server.sin_family = AF_INET;//绑定为ipv4
    server.sin_port = htons(port);//设置访问端口
    server.sin_addr.s_addr = inet_addr(host); //设置访问ip地址
    if ((connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)) {//连接失败
        //perror("connect()");
        return -1;//返回-1
    }
    return sockfd; //返回套接字
}

//获取日期时间的函数，返回一个字符串指针，用完释放
char *get_date_time() {
    char *str = malloc(sizeof(char) * 25);                      //申请一个字符串空间
    memset(str, 0, sizeof(char) * 25);                          //全置为0
    time_t timep;                                                                       //定义一个记录时间戳的变量
    struct tm *p;                                                                       //定义一个时间结构体指针
    time(&timep);                                                                       //获取时间戳
    p = gmtime(&timep);                                                         //获得时间结构体地址

    int year = 1900 + p->tm_year;                                       //获取当前年份,从1900开始，所以要加1900
    int mon = 1 + p->tm_mon;                                            //获取当前月份,范围是0-11,所以要加1
    int day = p->tm_mday;                                                       //获取当前月份日数,范围是1-31
    int hour = 8 + p->tm_hour;                                          //获取当前时,这里获取西方的时间,刚好相差八个小时
    int min = p->tm_min;                                                        //获取当前分
    int sec = p->tm_sec;                                                        //获取当前秒

    sprintf(str, "%d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec); //格式化到字符串

    return str;

}

//日志函数
void writ_log_file(int flag, const char *str) {
    FILE *fp = fopen(LOG_FILE, "a");                    //打开日志文件
    char *d_t = get_date_time();                        //获取日期时间字符串
    char *tmpbool = (flag ? "true" : "false");          //判断是错误还是正确日志
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

//输出学生信息函数
void print_msg(STU_M *stu) {

    erase();                    //清屏
    gui();                      //画框架
    int line = 2;
    char *str = (char *)malloc(sizeof(char) * 3 * MAX_LEN);
    
    
    sprintf(str, "%12s%12s", "学号", "姓名");                   //输出表头

    for (int i = 0; i < stu->course_num; i++) {                 //循环遍历课程名
        sprintf(str, "%s\t%s", str, stu->course_name[i]);       //输出课程名
    }
    sprintf(str, "%s\t%s\t%s", str, "总分", "平均分");          //格式化输出到str
        
    move(line, 4);                                              //移动
    addstr(str);                                                //添加到屏幕
    refresh();                                                  //刷新

    STU *tmp = stu->head;                                       //定义学生临时节点等于头节点

    //循环输出学生信息
    //while (tmp) {
    for (int i = 0; i < stu->stu_num; i++) {
        sprintf(str, "%10ld%10s", tmp->id, tmp->name);          //格式化输出到学号姓名到str
        //循环遍历分数
        for (int j = 0; j < stu->course_num; j++) {
            sprintf(str, "%s\t%.2lf", str, tmp->score[j]);      //格式化输出分数到str
        }
        sprintf(str, "%s\t%.2lf\t%.2f", str, tmp->sum, tmp->aver);   //格式化输出总分和平均分到str
        tmp = tmp->next;                                        //临时节点等于下一个节点
        ++line;
        move(line, 4);                                          //移动
        addstr(str);                                            //添加到屏幕
    }
    free(str);                          //释放str空间 
    ++line;                             //行+1                            
    ++line;                             //行+1
    color_print(line, 4, COLOR_BLUE, COLOR_BLACK, \
                "按任意键返回菜单", &color_flag);
    refresh();                          //刷新
    getch();                            //输入一个字符
    return ;
}

//字符串转换为整数
int str_to_int(char *str) {
    int ret = -1, tmp = 1;
    int len = strlen(str);
    if (len == 0) ret = -1;
    for (int i = len - 1; i >= 0; i--) {        //逆序遍历字符串
        if (!i) {
            ret += 1;
        }
        if (str[i] >= '0' && str[i] <= '9') {   //如果字符是0-9
            ret += tmp * (str[i] - '0');        //计算值
        } else {
            ret = -1;
            break;
        }
        tmp *= 10;                              //tmp翻十倍
    }
    return ret;
}

// 创建学生信息链表
void creat_student(STU_M *stu) {
    STU *tmp;                                       //定义一个临时学生结点
    STU *end = stu->head;                           //定义一个尾指针

    while (end->next != NULL) end = end->next;      //循环找到尾指针
    //循环添加信息
    for (int i = 0; i < stu->stu_num; i++) {
        tmp = (STU *)malloc(sizeof(STU *));         //给临时指针申请空间
        end->next = tmp;                            //赋值给临时指针的下个结点
        end = end->next;                            //尾指针移到尾部
        scanf("%ld", &(end->id));                   //输入学号
        scanf("%s", end->name);                     //输入姓名
        for (int j = 0; j < stu->course_num; j++) { //循环输入成绩
            scanf("%f", &(end->score[j]));          //输入成绩
        }
    }
        return ;
}

//创建学生链表函数
STU_M *creat(STU_M *stu) {
        STU *head;                          //定义一个头节点
        STU *p1, *p2;                       //定义两个节点
        int n = stu->stu_num;               //学生数赋值给n
        int m = stu->course_num;            //课程数赋值给m

        //循环申请空间
        for (int i = 1; i < n + 1; i++) {   
                p1 = (STU *)malloc(sizeof(STU)); //p1申请空间
                p1->next = NULL;                 //p1得下一个节点为空
                if (i == 1) {   
                        head = p2 = p1;         //p1赋值给p2和头节点
                }
                else {
                        p2->next = p1;
                        p2 = p1;
                }
        }
        stu->head = head;
        return stu;
}


//计算每门课程的总分和平均分
void  sum_avg_every_course(STU_M *stu) {
    erase();                //清屏
    gui();                  //画框架
    int line = 4;
    char *str = (char *)malloc(sizeof(char) * 2 * MAX_LEN);
    STU * head = stu->head; //头结点
    int n = stu->stu_num, m = stu->course_num; //n为学生数
    STU *p;                 //p用来遍历
    float sum;              //sum用来记总分
    p = head;               //p等于头结点

    sprintf(str, "每门课平均分和总分");
    move(line, (get_winsize(COL) - strlen(str))/ 2);
    addstr(str);
    //循环遍历并统计总分和平均分
    ++line;
    if (head != NULL) {     
        for (int i = 0; i < m; i++) {
            p = head;
            sum = 0.0;
            do {
                sum += p->score[i];         //循环累加总分
                p = p->next;
            } while (p != NULL);

            ++line;
            sprintf(str, "%s 课程:  总分=%.0f , 平均分=%.0f",\
                    stu->course_name[i], sum, sum / n);         //格式化输出到str
            move(line, (get_winsize(COL) - strlen(str)) / 2);   //移动
            addstr(str);                                        //添加到屏幕
            refresh();                                          //刷新
        }

    }
    line += 2;
    sprintf(str, "按任意键回到菜单");
    color_print(line, (get_winsize(COL) - strlen(str)) / 2,\
                COLOR_BLUE, COLOR_BLACK, str, &color_flag);
    getch();                //读取一个字符
    free(str);              //释放str
    str = NULL;             //str指向NULL，避免野指针
    return ;
}

//计算每个学生的总分和平均分
void  sum_avg_every_student(STU_M *stu) {
    STU *node = stu->head;                              //定义一个节点等于头节点
    while (node) {                                      //循环计算
        float sum = 0.0;                                //定义一个和变量
        for (int i = 0; i < stu->course_num; i++) {     //循环相加
            sum += node->score[i];                      
        }
        node->sum = sum;                                //节点总成绩赋值
        node->aver = sum / stu->course_num;             //节点平均成绩赋值
        node = node->next;                              //节点指向下一个节点
    }
    print_msg(stu);                                     //打印学生信息
    return ;
}

STU_M  *sort_by_score(STU_M *stu) {
    STU *head = stu->head;
    int n = stu->stu_num;
    STU *endpt;    //控制循环比较
    STU *p;        //临时指针变量
    STU *p1, *p2;

    p1 = (STU *)malloc(LEN);    //定义一个虚拟p1节点
    p1->next = head;            //p1下个节点指向头节点
    head = p1;                  //让head指向p1节点，排序完成后，我们再把p1节点释放掉

    //循环排序(冒泡排序)
    for (endpt = NULL; endpt != head; endpt = p) {      
        for (p = p1 = head; p1->next->next != endpt; p1 = p1->next) {
            if (p1->next->sum < p1->next->next->sum)  { //如果前面的节点键值比后面节点的键值大，则交换
                p2 = p1->next->next;
                p1->next->next = p2->next;
                p2->next = p1->next;
                p1->next = p2;
                p = p1->next->next;
            }
        }
    }

    p1 = head;                  //把p1的信息去掉
    head = head->next;          //让head指向排序后的第一个节点
    free(p1);                   //释放p1
    p1 = NULL;                  //p1置为NULL，避免野指针
    stu->head = head;
    return stu;
}

STU_M  *sort_by_score_reverse(STU_M *stu) {
    STU *head = stu->head;  //定义一个头节点
    int n = stu->stu_num;   //n为学生数
    STU *endpt;    //控制循环比较
    STU *p;        //临时指针变量
    STU *p1, *p2;

    p1 = (STU *)malloc(LEN);
    p1->next = head;        //定义一个虚拟节点
    head = p1;              //让head指向p1节点，排序完成后，我们再把p1节点释放掉

    //循环排序(冒泡排序)
    for (endpt = NULL; endpt != head; endpt = p) {
        for (p = p1 = head; p1->next->next != endpt; p1 = p1->next) {
            if (p1->next->sum > p1->next->next->sum)  {     //如果前面的节点键值比后面节点的键值大，则交换
                p2 = p1->next->next;
                p1->next->next = p2->next;
                p2->next = p1->next;
                p1->next = p2;
                p = p1->next->next;
            }
        }
    }

    p1 = head;              //把p1的信息去掉
    head = head->next;      //让head指向排序后的第一个节点
    free(p1);               //释放p1
    p1 = NULL;              //p1置为NULL，避免野指针
    stu->head = head;
    return stu;
}

STU_M *sort_by_num(STU_M *stu) {
    STU *head = stu->head;
    STU *first;    //为原链表剩下用于直接插入排序的节点头指针
    STU *t;        //临时指针变量：插入节点
    STU *p, *q;     //临时指针变量

    first = head->next;      //原链表剩下用于直接插入排序的节点链表：可根据图12来理解
    head->next = NULL;       //只含有一个节点的链表的有序链表：可根据图11来理解

    while (first != NULL) {  //遍历剩下无序的链表
        //注意：这里for语句就是体现直接插入排序思想的地方
        for (t = first, q = head; ((q != NULL) && (q->id < t->id)); p = q, q = q->next);  //无序节点在有序链表中找插入的位置

        first = first->next; //无序链表中的节点离开，以便它插入到有序链表中

        if (q == head) {     //插在第一个节点之前
            head = t;
        }
        else {           //p是q的前驱
            p->next = t;
        }
        t->next = q;     //完成插入动作
                                             //first = first->next;
    }
    stu->head = head;
    return stu;
}

STU_M  *sort_by_name(STU_M *stu) {
    int n = stu->stu_num;   //n为学生数
    STU *head = stu->head;  //头节点
    STU *endpt;    //控制循环比较
    STU *p;        //临时指针变量
    STU *p1, *p2;

    p1 = (STU *)malloc(LEN);
    p1->next = head;            //定义一个虚拟节点
    head = p1;                  //让head指向p1节点，排序完成后，再把p1节点释放掉

    //循环排序
    for (endpt = NULL; endpt != head; endpt = p) {
        for (p = p1 = head; p1->next->next != endpt; p1 = p1->next) {
            if (strcmp(p1->next->name, p1->next->next->name)>0) { //如果前面的节点键值比后面节点的键值大，则交换
                p2 = p1->next->next;
                p1->next->next = p2->next;
                p2->next = p1->next;
                p1->next = p2;     
                p = p1->next->next;   
            }
        }
    }

    p1 = head;                  //把p1的信息去掉
    head = head->next;          //让head指向排序后的第一个节点
    free(p1);                   //释放p1
    p1 = NULL;                  //p1置为NULL，避免野指针
    stu->head = head;

    return stu;
}



void  search_by_Num(STU_M *stu) {
    erase();                            //清屏
    gui();                              //画框架
    refresh();                          //刷新
    STU *head = stu->head;              //定义一个头节点
    int n = stu->stu_num;               //学生数赋值给n
    int m = stu->course_num;            //课程数赋值给m
    long num;                           //定义一个学号变量
    char *in = (char *)malloc(sizeof(char) * 2 * MAX_LEN); //定义一个行数变量并申请一个字符串空间
    int line = 2;                       //定义个行变量
    int flag = 1;                       //定义一个标志

    move(line, 4);                      //移动
    addstr("情输入学号:");               //添加字符串到屏幕
    refresh();                          //刷新
    color_print(line, 12, COLOR_BLUE, COLOR_BLACK, INTIP, &color_flag); //颜色输出
    getstr(in);                         //输入in
    num = atol(in);                     //字符串转整形
    STU *p;                             //定义一个学生指针变量
    p = head;                           //p等于头节点
    ++line;                             //行+1
    ++line;                             //行+1
    if (head != NULL) {                 //循环查找
        do {
            if (p->id == num) {         //判断学号是否等于输入得学号
                sprintf(in, "%ld\t%s\t", p->id, p->name); //姓名格式化输出到in
                for (int i = 0; i<m; i++) {
                        sprintf(in, "%s\t%.2f\t", in, p->score[i]); //成绩格式化输出到in
                }
                sprintf(in, "%s\t%.2f\t%.2f\n", in, p->sum, p->sum / m); //总分,平均分格式化输出到in
                move(line, 4);          //移动
                addstr(in);             //添加到屏幕
                flag = 0;               //flag标记0
            }
            p = p->next;                //p指向下一个节点
        
        } while (p != NULL);
        if (flag) {                     //如过未找到
            move(line, 4);              //移动
            addstr("未找到");           //添加到屏幕
        }
    }    
    line += 2;                             //行+2
    color_print(line, 4, COLOR_GREEN, COLOR_BLACK, \
                "按任意键回到菜单", &color_flag);

    refresh();                          //刷新
    getch();                            //获取一个字符输入
    free(in);                           //释放in得空间

}

void  search_by_Name(STU_M *stu) {
    erase();                        //清屏
    gui();                          //画框架
    int line = 2;                   //定义行变量
    int n = stu->stu_num;           //学生数赋值给n
    int m = stu->course_num;        //课程数赋值给m
    char name[2 * MAX_LEN];             //定义一个学生姓名变量
    int flag = 1;                   //定义一个标志等于1

    move(line, 4);                  //移动
    color_print(line, 4, COLOR_BLUE, COLOR_BLACK, INTIP, &color_flag);
    ++line;                         //行+1
    refresh();                      //刷新
    getstr(name);                   //输入
    STU *p;                         //定义一个节点p
    p = stu->head;                       //p等于头节点

    //循环查找
    while (p != NULL) {
        if (strcmp(name, p->name) == 0) {               //如果找到
            sprintf(name, "%ld\t%s\t", p->id, p->name); //格式化姓名输出到name
            for (int i = 0; i<m; i++) {                 //循环输出成绩
                sprintf(name, "%s\t%.2f\t", name, p->score[i]);     //格式化输出成绩到name
            }
            sprintf(name, "%s\t%.2f\t%.0f\n", name, p->sum, p->sum / m); //格式化输出总分平均分到name
            flag = 0;
            break;
        }
        p = p->next;

    }
    if (flag) {                 //如果为未找到
        ++line;
        color_print(line, 4, COLOR_RED, COLOR_BLACK, "未找到", &color_flag);     //颜色输出
    } else {
        ++line;                                         //行+1
        move(line, 4);                                  //移动
        addstr(name);                                   //添加到屏幕
    }
    line += 2;                                          //行+2
    color_print(line, 4, COLOR_GREEN, COLOR_BLACK, \
                "按任意键回到菜单", &color_flag);       //颜色输出
    refresh();                                          //刷新
    getch();
    return ;
}

void  statistic_analysis(STU_M *stu) {
    erase();                        //清屏
    gui();                          //话框架
    int line = 2;                   //定义一个行数变量
    int n = stu->stu_num;           //n为学生数
    int m = stu->course_num;        //m为课程数
    int a[6] = {0};                 //数组存放结果
    STU *p;                         //结点p用来遍历
    char *str = (char *)malloc(sizeof(char) * 2 * MAX_LEN); //申请一个字符串用来记录信息
    p = stu->head;                  //p结点等于头结点   
    double cnt = 0.0;               //cnt用来记录总人数
    
    for (int i = 0; i < m; i++) {
        p = stu->head;              //每次开始前初始化p结点
        for (int j = 0; j < 6; j++) {//初始化
            a[j] = 0;
        }
        cnt = 0.0;
        //循环统计
        do {
            if (p->score[i]<60) {       //如果小于60分
                a[0]++, cnt++;
            }
            else if (p->score[i]<70) {  //如果小于70分
                a[1]++, cnt++;
            }
            else if (p->score[i]<80) {  //如果小于80分
                a[2]++, cnt++;
            }
            else if (p->score[i]<90) {  //如果小于90分
                a[3]++, cnt++;
            }
            else if (p->score[i]<100) { //如果小于100分
                a[4]++, cnt++;
            }
            else {                      //否则100分
                a[5]++, cnt++;
            }

            p = p->next;
        } while (p != NULL);
        
        move(line, get_winsize(COL) / 2 - 8);
        addstr("统计如下");
        line += 2;                          //行+2
        int right = get_winsize(COL) - 1;   //右边框
        for (int i = 1; i < right; i++) {   //循环输出操作
            move(line, i);                  //移动
            addch('-');                     //添加'-'
        }
        line += 2;

        sprintf(str, "* For %s:", stu->course_name[i]);
        move(line,(get_winsize(COL) - strlen(str)) / 2 - 5);
        addstr(str);
        refresh();

        line += 2;
        sprintf(str, "* <60\t%d\t%.2f%%", a[0], 100 * a[0] / cnt);
        color_print(line, (get_winsize(COL) - strlen(str)) / 2 - 10, COLOR_RED, COLOR_BLACK, str, &color_flag);
        refresh();
        
        ++line;
        sprintf(str, "* %d-%d\t%d\t%.2f%%", 60, 69, a[1], 100 * a[1] / cnt);
        color_print(line, (get_winsize(COL) - strlen(str)) / 2 - 10, COLOR_MAGENTA, COLOR_BLACK, str, &color_flag);
        refresh();

        ++line;
        sprintf(str, "* %d-%d\t%d\t%.2f%%", 70, 79, a[2], 100 * a[2] / cnt);
        color_print(line, (get_winsize(COL) - strlen(str)) / 2 - 10, COLOR_YELLOW, COLOR_BLACK, str, &color_flag);
        refresh();

        ++line;
        sprintf(str, "* %d-%d\t%d\t%.2f%%", 80, 89, a[3], 100 * a[3] / cnt);
        color_print(line, (get_winsize(COL) - strlen(str)) / 2 - 10, COLOR_CYAN, COLOR_BLACK, str, &color_flag);
        refresh();

        ++line;
        sprintf(str, "* %d-%d\t%d\t%.2f%%", 90, 99, a[4], 100 * a[4] / cnt);
        color_print(line, (get_winsize(COL) - strlen(str)) / 2 - 10, COLOR_BLUE, COLOR_BLACK, str, &color_flag);
        refresh();
        
        ++line;
        sprintf(str, "* %d\t%d\t%.2f%%", 100, a[5],  100 * a[5] / cnt);
        color_print(line, (get_winsize(COL) - strlen(str)) / 2 - 10, COLOR_GREEN, COLOR_BLACK, str, &color_flag);
        refresh();

        ++line;

    }
    ++line;
    sprintf(str, "按任意键返回菜单");
    color_print(line, (get_winsize(COL) - strlen(str)) / 2, \
                COLOR_BLUE, COLOR_BLACK, "按任意键返回菜单", &color_flag);
    free(str);
    getch();

    return ;
}


//输入学生信息
int get_stu_msg(STU_M *stu) {
    erase();                                    //清屏
    gui();                                      //画框架
    char *in = (char *)malloc(sizeof(char) * MAX_LEN); //定义一个变量用于输入
    int line = 2;                               //定义一个行数变量
    STU *node, *tmp;                            //定义一个学生结点和临时结点
    int n = 0;
    int m = 0;

    move(line, 4);                              //移动
    addstr("请输入要插入学生数: ");             //输入学生数
    move(line, 25);                             //移动
    refresh();                                  //刷新
    getstr(in);                                 //输入
    n = str_to_int(in);                         //字符串转整数
    while (n < 0) {
        color_print(line + 1, 4, COLOR_RED, COLOR_BLACK, "(输入错误，请重新输入 学生数不能小于0)", &color_flag);
        move(line, 25);                             //移动
        addstr(menu_str[0]);
        move(line, 25);                             //移动
        refresh();                                  //刷新
        getstr(in);                                 //输入
        n = str_to_int(in);                         //字符串转整数
    } 
    if (n == 0) {
        free(in);
        return 0;                       //为0返回0
    }
    stu->stu_num += n;
    int new_stu = n;
    int new_course = stu->course_num;

    color_print(line + 1, 4, COLOR_RED, COLOR_BLACK, "(输入正确", &color_flag);      //颜色输出

    if (stu->stu_num == 0) {
        ++line;
        move(line, 4);                              //移动
        addstr("请输入课程数: ");                   //添加
        move(line, 21);                             //移动
        refresh();                                  //刷新
        getstr(in);                                 //输入
        n = str_to_int(in);                         //字符串转整型
        while (n <= 0) {
            color_print(line + 1, 4, COLOR_RED, COLOR_BLACK, "(输入错误，课程数大于0)", &color_flag);
            move(line, 21);                             //移动
            addstr(menu_str[0]);
            move(line, 21);                             //移动
            refresh();                                  //刷新
            getstr(in);                                 //输入
            n = str_to_int(in);                         //字符串转整型
        }
        stu->course_num = n;                            //初始化课程数
        new_course = 0;                                 
        move(line + 1, 4);
        addstr(menu_str[0]);                            //清空
        color_print(line + 1, 4, COLOR_GREEN, COLOR_BLACK, "(输入正确)", &color_flag);
        
        //开辟课程名空间
        stu->course_name = (char **)malloc(sizeof(char *) * stu->course_num);
        for (int i = 0; i < stu->course_num; i++) {
            stu->course_name[i] = (char *)malloc(sizeof(char) * MAX_LEN); //申请空间
        }

        ++line;
        move(line, 4);                                      //移动
        sprintf(in, "请输入%d门课程名", stu->course_num);   //格式化输出到in
        addstr(in);                                         //添加到屏幕

        //循环输入课程名
        for (int j = 0; j < stu->course_num; j++) {
            refresh();                                      //刷新
            getstr(stu->course_name[j]);                    //输入课程名
            ++line;                                         //行+1
            move(line, 4);                                  //移动
        }

        stu->head = (STU *)malloc(sizeof(STU));             //学生管理结构体申请空间
            node = stu->head;                               //结点等于学生头结点
        } else {
        tmp = node = stu->head;                             //临时结点等于结点
        while (tmp->next) {
            tmp = tmp->next;                                //找到尾结点
        }
        tmp->next = (STU *)malloc(sizeof(STU));             //给临时结点的下一个结点申请空间
        node = tmp->next;                                   
    }



    node->next = NULL;                                      //结点置空

    ++line;
    move(line, 4);                                          //移动
    addstr("请按照以下格式输入：");                         //添加到屏幕
    
    sprintf(in, "学号\t姓名");                              //格式化输出到in
    for (int i = 0; i < stu->course_num; i++) sprintf(in, "%s\t%s", in, stu->course_name[i]); //循环格式化输出到in
    ++line;                                                 //行+1
    move(line, 4);                                          //移动
    addstr(in);                                             //添加到屏幕



    color_print(line, strlen(in) * 2, COLOR_RED, COLOR_BLACK, "输入\"no\"回车取消录入，输入其他回车开始录入: ", &color_flag); //有颜色输出
    
    //判断是否取消录入
    getstr(in);
    if ((strcmp(in, "no") == 0) || (strcmp(in, "No") == 0)) {   //如果输入是"no"
        stu->stu_num -= new_stu;                            //学生数减new_stu
        stu->course_num = new_course;                       //课程数等于new_course
        free(in);
        return 0;
    }



    ++line;
    move(line, 4);                                      //移动
    refresh();                                          //刷新
    getstr(in);                                         //输入
    node->id = atoi(in);                                //字符转整型
 
    ++line;
    move(line, 4);                                      //移动
    refresh();                                          //刷新
    getstr(node->name);                                 //输入

    //循环输入课程名
    for (int i = 0; i < stu->course_num; i++) {   
        ++line; 
        move(line, 4);                                  //移动                               
        refresh();                                      //刷新
        getstr(in);                                     //输入
        node->score[i] = atof(in);              
    }

    //循环输入学生信息
    for (int i = 1; i < n; i++) {                   
        node->next = (STU *)malloc(sizeof(STU));        //给下一个结点申请空间
        node = node->next;                              //结点等于下一个结点
        node->next = NULL;                              //下一个结点置空
        ++line;     
        move(line, 4);                                  //置空
        refresh();                                      //刷新
        getstr(in);                                     //输入
        node->id = str_to_int(in);                      //给学号赋值
 
        ++line;
        move(line, 4);                                  //移动
        refresh();                                      //刷新
        getstr(node->name);                             //给姓名赋值

        //循环输入成绩
        for (int j = 0; j <stu->course_num; j++) {
            ++line;
            move(line, 4);                              //移动
            refresh();                                  //刷新
            getstr(in);                                 //输入
            node->score[j] = atof(in);                  //给成绩赋值
        }
    }
    free(in);
    return 1;
}

//获取配置文件
int get_conf_value(char *file, char *key, char *val) {
    int ret = -1;                               //定义一个返回值
    FILE *fp = NULL;                            //定义文件指针
    char *line = NULL, *substr = NULL;          //定义行指针,子字符串指针
    size_t n = 0, len = 0;                      //定义n，长度len
    ssize_t read;                               //定义一个记录返回长度的变量

    if (key == NULL) {                          //如果key为空
        writ_log_file(FALSE, "key错误");        //写日志
        return ret;
    }
    fp = fopen(file, "r");                      //以只读打开文件

    if (fp == NULL) {                           //打开失败
        writ_log_file(FALSE, "打开配置文件错误");   //写日志
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
        char tmp[30];
        sprintf(tmp, "在%s中未找到%s配置文件", file, key);
        writ_log_file(FALSE, tmp);
        ret = -1;
    }
    fclose(fp);                                 //关闭文件
    free(line);                                 //释放line
    return 0;
}

//按学号删除函数
int on_id_del(STU_M *stu) {
    erase();                                    //清屏
    gui();                                      //画框架
    STU *head = (STU *)malloc(sizeof(STU));     //定义一个学生虚拟头结点
    head->next = stu->head;                     //虚拟头结点的下个结点等于学生实际头结点
    STU *node = head;                           //定义一个结点等于虚拟头结点
    STU *tmp;                                   //定义一个临时结点用来标记删除的结点
    char *str = (char *)malloc(sizeof(char) * MAX_LEN);  //str用来输入
    int line = 2;                               //定义一个行数变量
    int flag = 1;                               //定义一个判断标志
    int num, ret = FALSE;                       //定义输入变量和返回值

    move(line, 4);
    addstr("请输入要删除的学号:");              //添加到屏幕
    move(line, 24);
    getstr(str);                                //输入            
    refresh();                                  //刷新
    num = atoi(str);                            //字符串转换为整数   

    //循环查找
    while (node->next) {
        if (node->next->id == num) {            //如果找到
            tmp = node->next;                   //临时结点标记
            if (stu->head == tmp) {             //如果删除的是头结点
                stu->head = tmp->next;          //结点后移一位
            }
            node->next = tmp->next;             //该结点越过要删除的结点
            free(tmp);                          //释放结点
            ret = TRUE;                         //标记返回值成功删除
            stu->stu_num--;
            line += 2;
            move(line, 4);
            color_print(line, 4, COLOR_GREEN, COLOR_BLACK, "删除成功", &color_flag); //颜色输出
            flag = 0;                           //标记标志
            break;                              //跳出循环
        }
        node = node->next;
    }
    if (flag) {
        line += 2;
        color_print(line, 4, COLOR_RED, COLOR_BLACK, "没有该学号的学生", &color_flag); //颜色输出
    }
    free(head);                                 //释放虚拟头结点
    head = NULL;                                //头节点置空，避免野指针
    free(str);                                  //释放str空间
    str = NULL;                                 //str制空，避免野指针
    line += 2;                                  //行+1
    color_print(line, 4, COLOR_BLUE, COLOR_BLACK, \
                "按任意键返回菜单", &color_flag);
    refresh();
    getch();
    return ret;
}

//按姓名删除函数
int on_name_del(STU_M *stu) {
    erase();                                    //清屏
    gui();                                      //画框架
    STU *head = (STU *)malloc(sizeof(STU));     //定义一个学生虚拟头结点
    head->next = stu->head;                     //虚拟头结点的下个结点等于学生实际头结点
    STU *node = head;                           //定义一个结点等于虚拟头结点
    STU *tmp;                                   //定义一个临时结点
    int ret = TRUE;                            //定义一个返回变量
    int line = 2;                               //定义一个行数变量
    char *str = (char *)malloc(sizeof(char) * MAX_LEN); //定义一个字符串用于输入
    move(line, 4);                              //移动
    addstr("请输入要删除的名字: ");             //添加到屏幕
    refresh();                                  //刷新
    getstr(str);                                //输入
    while (node->next) {                        //循环查找
        if (strcmp(node->name, str) == 0) {     //如果找到
            tmp = node->next;                   //临时结点标记
            if (stu->head == tmp) {             //如果删除的是头结点
                stu->head = tmp->next;          //头结点往后移动一位
            }
            node->next = tmp->next;             //结点越过要删除的结点
            free(tmp);                          //释放删除的结点
            ret = TRUE;                         //标记返回值删除成功
            stu->stu_num--;
            line += 2;
            color_print(line, 4, COLOR_GREEN, COLOR_BLACK, "删除成功", &color_flag);
            ret = FALSE;
            break;                              //跳出循环
        }
        node = node->next;
    }
    if (ret) {
        line += 2;
        color_print(line, 4, COLOR_GREEN, COLOR_BLACK, "没有该学生", &color_flag);
    }
    free(head);                                 //释放虚拟头结点
    head = NULL;                                //head置空，避免野指针
    free(str);                                  //释放字符串变量
    str = NULL;                                 //str制空，避免野指针
    line += 2;
    color_print(line, 4, COLOR_BLUE, COLOR_BLACK, \
                "按任意键返回菜单", &color_flag);
    refresh();
    getch();

    return ret;
}

//删除全部学生函数
int all_del(STU_M *stu) {
    erase();                                                //清屏
    gui();                                                  //画框架
    char *dstr = (char *)malloc(sizeof(char) * MAX_LEN);    //定义一个确认字符串
    int line = 5;                                           //定义一个行数变量
    color_print(line, 6, COLOR_RED, COLOR_BLACK,\
    "确定删除全部学生信息？请输入: (\"1\"或\"yes\" 确定,任意键取消)", &color_flag);   //颜色输出
    ++line;
    color_print(line, 6, COLOR_RED, COLOR_BLACK, INTIP, &color_flag);   //颜色输出
    getstr(dstr);                                      //输入是否删除

    //删除
    if ((strcmp(dstr, "yes") == 0) || (strcmp(dstr, "1") == 0)) {                                         //如果删除
        STU *head = stu->head;                              //定义一个头结点
        STU *tmp;                                           //定义一个临时结点
        stu->head = NULL;                                   //学生管理结构体头结点置空
        stu->stu_num = 0;                                   //学生数归零
        stu->course_num = 0;                                //课程数归零
        while(head) {                                       //遍历结点
            tmp = head;                                     //赋予临时结点
            head = tmp->next;                               //头结点指向下一个结点
            free(tmp);                                      //释放临时结点
        }
        line += 2;
        color_print(line, 6, COLOR_RED, COLOR_BLACK, "已删除", &color_flag);

    } else {
        line += 2;
        color_print(line, 6, COLOR_GREEN, COLOR_BLACK, "已取消", &color_flag);
    }
    ++line;
    color_print(line, 6, COLOR_BLUE, COLOR_BLACK, \
                "按任意键返回菜单", &color_flag);
    refresh();
    getch();
    free(dstr);                                             //释放字符串
    dstr = NULL;                                            //str置空，避免野指针
    return 0;
}

//发送数据给服务器
int send_stu_data(STU_M *stu, const int sockfd) {
    int send_v;                                             //定义一个返回值

    send_v = send(sockfd, stu, sizeof(STU_M), 0);           //发送学生管理结构体
    if (send_v <= 0) {
        writ_log_file(FALSE, "发送学生管理结构体");         //写日志
    }
	//循环发送课程名
    for (int i = 0; i < stu->course_num; ++i) {
        if (send(sockfd, stu->course_name[i], MAX_LEN, 0) <= 0) { //发送课程名
            writ_log_file(FALSE, "发送课程名");             //写日志
        }
    }

    STU *node = stu->head;                                  //定义一个节点
    int len = sizeof(STU);                                  //算出学生结构体大小
    for (int i = 0; i < stu->stu_num; i++) {                //序号发送学生数据
        if (i) {
            node = node->next;                              //如果不是第一次，节点等于下一个节点
        }
        send_v = send(sockfd, node, len, 0);                //发送节点
        if (send_v <= 0) {
            writ_log_file(FALSE, "send error\n");           //写日志
        }
    }
    return 1;
}

//给服务器发送0，关闭连接
void over(int sockfd) {
    int val = 0;
    send(sockfd, &val, sizeof(val), 0); //发送0
    close(sockfd);                      //关闭套接字
    return ;
}


//菜单
void menu() {
    int len = sizeof(menu_str[1]);      //计算长度
    for (int i = 1; i < 17; i++) {      //循环遍历菜单字符串
        move(i, 3);                 //移动
        addstr(menu_str[i]);            //添加
    }
    int right = get_winsize(COL) - 1;   //右边框
    for (int i = 1; i < right; i++) {   //循环输出操作
        move(17, i);                    //移动
        addch('-');                     //添加'-'
    }
    return ;
}

//添加有颜色的输出
void color_print(int x, int y, int foreground, int background, char *p, int *flag) {	
    if(start_color() == OK) {                     //判断终端是否支持有颜色输出
	    init_pair(*flag, foreground, background);   //初始化颜色，前景、背景

	    attron(COLOR_PAIR(*flag));                  //开始有颜色输出
	    move(x, y);                             //移动到x,y
	    addstr(p);                              //绘制
	    attroff(COLOR_PAIR(*flag));                 //结束有颜色输出
        ++(*flag);
    } else {
	    move(x, y);                             //移动到x,y
	    addstr(p);                              //绘制
    }
}

//获取窗口尺寸函数
int get_winsize(int flag) {
    struct winsize size;
    int ret = 0;
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);   //初始化窗口尺寸结构体

    switch (flag) {
        case COL: { //flag == COL 返回高度
            ret = size.ws_col;
            break;
        }
        case ROW: { //flag == ROW 返回宽幅
            ret = size.ws_row;
            break;
        }
        default : { //否则返回-1(传入错误flag)
            ret = -1;
        }
    }
    return ret; 
}

//页面框架
void gui() {
    box(stdscr,'|','-');    //'|'竖边框,'-'横边框
    char *stu_str = "学生信息管理系统";     //标题
    int mid = (get_winsize(COL) - strlen(stu_str)) / 2 - 1; //中点
    move(0, mid);   //移动
    addstr("学生信息管理系统"); //添加字符串
}
