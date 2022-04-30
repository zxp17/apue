/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(19/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "19/04/22 09:17:43"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <libgen.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <sqlite3.h>
#include "gettime.h"
#include "create_database.h"

#define	MAX_EVENTS		512

int socket_server_init(char *listen_ip,int listen_port);
static inline void print_usage(char *progname);
void set_socket_rlimit(void);


int main(int argc,char ** argv)
{

	int					listenfd,connfd;
//	char				*listenip;
	int					serv_port = 0;
	int					daemon_run = 0;
	int					epollfd = 0;
	struct epoll_event	event;
	struct epoll_event	event_array[MAX_EVENTS];
	int					events;
	int					i;
	int					x = 0,y = 0;
	int					rv;
	char				buf[1024];
	int					opt;
	char				*progname = NULL;


	//将获取得到的数据存在take_info二元数组里面
	char				*p = NULL;
	char				*p1 = (char*)malloc(1024);
	char				take_info[32][256] = {0};

	//执行时的sql语句
	char				sql[2048];
	int					rc;
	sqlite3				*db;
	
	struct option		long_options[] = 
	{
		{"daemon",no_argument,NULL,'d'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0},
	};


	progname = basename(argv[0]);


	//解析命令行参数
	while((opt = getopt_long(argc,argv,"dp:h",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'd':
				daemon_run = 1;
				break;
			case 'p':
				serv_port = atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;
		}
	}

	if(!serv_port)
	{
		print_usage(progname);
		return -1;
	}


	set_socket_rlimit();


	if((listenfd = socket_server_init(NULL,serv_port)) < 0)
	{
		printf("BAD NEWS:  server listen on port %d failure\n",serv_port);
		return -2;
	}

	printf("listenfd = %d\n",listenfd);
	printf(" server start to listen on port %d\n",serv_port);



	if( daemon_run )		//set program running on background
	{
		daemon(0,0);
		//第一个参数为0，表示将进程的工作目录改为‘/’根目录
		//第二个参数为0，表示输入、输出以及错误输出重定向到/dev/null
	}


	//epoll_create()该函数生成一个epoll专用的文件描述符，其中参数是指定生成描述符的最大范围
	if((epollfd = epoll_create(MAX_EVENTS)) < 0)
	{
		printf("epoll_create() failure: %s\n",strerror(errno));
		return -3;
	}

	//event.events = EPOLLIN|EPOLLET;
	event.events = EPOLLIN;			//EPOLLIN表示对应的文件描述符可以读
	event.data.fd = listenfd;		//保存触发事件的某个文件描述符



	//epoll_ctl()函数用于控制某个文件描述符上的事件
	//参数EPOLL_CTL_ADD表示注册事件
	if( epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event) < 0 )
	{
		printf("epoll add listen socket failure: %s\n",strerror(errno));
		return -4;
	}



	for(;;)
	{

		//program will blocked here

		//epoll_wait()该函数用于轮询I/O事件的发生
		//event_array是用于回传处理事件的数组
		//MAX_EVENTS是每次能处理的事件数
		//最后一个参数-1表示调用将一直阻塞，直到兴趣列表中的文件描述符上有事件产生或者直到捕捉到一个信号为止
	
		events = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);

		if(events < 0)
		{
			printf("epoll_wait failure: %s\n",strerror(errno));
			break;
		}
		else if(0 == events)		//也许这一步可以省略掉
		{
			printf("epoll_wait get timeout\n");
			continue;
		}

		//events > 0 的情况，往下运行
		for(i = 0; i < events; i++)
		{
			//当事件对应的文件描述符发生错误或者是被挂断的时候，输出错误提示信息
			if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]: %s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
		
				//epoll_ctl()该函数用于控制文件描述符上的文件，EPOLL_CTL_DEL表示删除事件
				close(event_array[i].data.fd);
			}
			
			//如果事件没出错，就开始准备accept
			if(event_array[i].data.fd == listenfd)
			{
				if((connfd = accept(listenfd,(struct sockaddr *)NULL,NULL)) < 0)
				{
					printf("accept new client failure: %s\n",strerror(errno));
					continue;
				}
				printf("accept成功\b");

				event.data.fd = connfd;
				event.events = EPOLLIN;			//表示对应的文件描述符可读
				//将该事件添加到兴趣列表中去

				if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event) < 0)
				{
					printf("epoll add client socket failure: %s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new client socket[%d] ok\n",connfd);
			}

			else		//already connected client socket get data incoming
			{
				memset(buf,0,sizeof(buf));

				if((rv = read(event_array[i].data.fd,buf,sizeof(buf))) <= 0)
				{
					printf("socket[%d] read failure or get disconnect and will be removed\n",event_array[i].data.fd);
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{

					printf("\n\n");

					printf("socket[%d] read get %d bytes data from client and echo back: %s\n",event_array[i].data.fd,rv,buf);


					//在这里要得到插入数据库的数据

					p = buf;
					x = 0;
					while((p1 = strchr(p,'\n')) != NULL)
					{
						
						strncpy(take_info[x],p,strlen(p) - strlen(p1));
						p = p1+1;
						x++;
					}
					//printf("x = %d\n",x);
					strncpy(take_info[x],p,strlen(p));

					for(y = 0;y <= x; y++)
					{
						printf("take_info[%d]: %s\n",y,take_info[y]);
					}
 
					//free(p1);

					open_database("temperature_database.db",&db);

					snprintf(sql,sizeof(sql),"INSERT INTO %s(SERIAL,TIME,TEMPERATURE)VALUES('%s','%s','%s');","COMPANY",take_info[0],take_info[1],take_info[2]);

					execute_exec(db,sql);

					close_database(db);


					if(write(event_array[i].data.fd,buf,rv) < 0)
					{
						printf("sokcet[%d] write failure: %s\n",event_array[i].data.fd,strerror(errno));
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
					}
					printf("write finish\n");
				}
			}
		}
	}
cleanup:
	close(listenfd);
	return 0;
}

int socket_server_init(char *listen_ip,int listen_port)
{
	struct	sockaddr_in		serveraddr;
	int						rv = 0;
	int						on = 1;
	int						listenfd;
	
	//创建套接字
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("create socket failure: %d\n",strerror(errno));
		return -1;
	}
	printf("create socket sucessfully\n");

	//setsockopt()函数用于任意类型、任意状态套接字的设置选项值
	//第二个参数表示被设置的选项的级别，想要套接字级别上设置选项，就必须把level设置为SOL_SOCKET
	//第三个参数表示设置的选项值SO_REUSEADDRR表示允许将套接字绑定到已使用的地址
	//第四个参数指向存放选项值的缓冲区
	//
	//set socket port reuseable,fix 'address already in use' bug when socket server restart
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;		//设定协议族为AF_INET
	serveraddr.sin_port = htons(listen_port);//将端口从主机字节序转成网络字节序

	if( !listen_ip )	//如果没有指定端口号，就监听所有的端口号
	{
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		printf("i am listening all ports\n");
	}
	else	//否则就监听指定的端口号
	{

		//inet_pton()函数，ip地址转换函数，可以将ip地址在“点分十进制”和“二进制整数之间转换”
		if(inet_pton(AF_INET,listen_ip,&serveraddr.sin_addr) <= 0)
		{
			printf("inet_pton() set listen ip failure\n");
			rv = -2;
			goto cleanup;
		}
	}

	//对套接字进行地址和端口的绑定，这一才能进行数据的接收和发送操作
	if(bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		printf("bind tcp socket failure: %s\n",strerror(errno));
		rv = -3;
		goto cleanup;
	}
	printf("bind()成功\n");

	//listen()函数中的第二个参数规定了内核应为套接字排队的最大连接个数
	if(listen(listenfd,77) < 0)
	{
		printf("listen failure: %s\n",strerror(errno));
		rv = -4;
		goto cleanup;
	}
	printf("listen成功\n");


cleanup:
	if( rv< 0 )
		close(listenfd);
	else
		rv = listenfd;

	return rv;

}
static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTION]...\n",progname);

	printf("%s is a socket server program,which used to verify client and echo back string from it\n",progname);

	printf("\nmandatory arguents to long options and short options\n");

	printf("-d[daemon ] set program running on background\n");
	printf("-p[port ] socket server port address\n");
	printf("-h[help ] display some information\n");

	printf("\n example: %s -d -p 7777\n",progname);

	return ;
}

//设置文件描述符的最大数目
void set_socket_rlimit(void)
{
	struct rlimit 	limit = {0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);

	printf("set socket open fd max count to %ld\n",limit.rlim_max);
}

