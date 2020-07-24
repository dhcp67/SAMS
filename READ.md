<h1 style="text-align:center">SAMS

-----------

**名称**： SAMS(学生成绩管理系统)

**源码地址**：  https://github.com/trainlee/1.CProject/tree/master/SAMS

 **作者**：Aleen Lee



----------



### 简介

* 基于C/S架构的学生成绩管理系统

* 文件存储数据/日志

* 文件读取配置

* 服务端是守护线程，静默运行，客户端是交互式运行

* ncursesw库设计UI

  

###运行环境

* linux

* 安装所需要的库：``apt-get install libncurses5-dev``

其他发行版自行搜索安装



### 项目组成

文件目录树如下

.
├── READ.ME
├── client
│   ├── Makefile
│   ├── ccommon.c
│   ├── ccommon.h
│   ├── ccommon.o
│   ├── client
│   ├── client.c
│   ├── client.conf
│   ├── client.log
│   └── client.o
└── server
    ├── Makefile
    ├── scommon.c
    ├── scommon.h
    ├── scommon.o
    ├── server
    ├── server.c
    ├── server.conf
    ├── server.log
    ├── server.o
    └── student.txt



* client文件夹存放客户端代码

  * ccommon.c

  * ccommon.h

  * ccommon.o

  * client

  * client.c

  * client.conf

  * client.log

  * client.o

* server文件夹存放服务端代码

  * Makefile

  * scommon.c

  * scommon.h

  * scommon.o

  * server

  * server.c

  * server.conf

  * server.log

  * server.o

  * student.txt



​		clien和server为可执行文件，执行``make``生成；client.c和server.c是客户端和服务端主函数在的文件；ccommon.h、scommon.h是头文件，存放需要用到的宏定义和函数声明；ccommon.c、scommon.c是需要用到的各种功能的实现的源文件。

​		client.conf、server.conf是存放配置的文件，由程序运行时读入，调用函数是``int get_conf_value(char *file, char *key, char *val)``;client.log、server.log是日志文件，程序运行时写入的日志，由函数``void writ_log_file(int , const char *)``写入。具体实现在ccomon.c、scommon.c文件中。*.o文件是二进制文件，由``make -c *.c``生成，但暂不能运行，因为未经过连接需要经过``gcc -o *.o``链接过后生成可执行文件方能运行。student.txt是存放学生数据的文件，由服务端操作。