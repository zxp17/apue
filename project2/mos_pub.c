/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mos_pub.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/05/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "16/05/22 15:04:07"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <mosquitto.h>
#include <signal.h>
#include <time.h>
#include "mos_pub.h"
#include "gettime.c"
#include "gettemper.c"
#include "client_database.c"

#define		HOST			"localhost"
#define		PORT			1883
#define		KEEP_ALIVE		60
#define		MSG_MAX_SIZE	512

#define		NUMBER			"777"

//定义运行标志决定是否需要结束
static int running = 1;
static int g_sig_out = 0;

void my_connect_callback(struct mosquitto *mosq,void *obj,int rc)
{
	printf("callback: connect successfully\n");
}

//rc :a value of 0 means the client has called mosquitto_disconnect.any other value indicates that the disconnect is unexpected
void my_disconnect_callback(struct mosquitto *mosq,void *obj,int rc)
{
	printf("callback: disconnected!!\n");
	running = 0;
}

void my_publish_callback(struct mosquitto *mosq,void *obj,int mid)
{
	printf("callback: publish called!!\n");
}

void print_usage(const char *program_name);
void sig_out(int signum);

int main(int argc,char *argv[])
{
	int					ret;
	struct mosquitto	*mosq;
//	struct mosquitto	mosq1;
	char				buff[MSG_MAX_SIZE];

	struct hostent		*hostnp;
	char				*ip = NULL;
	int					retain = 0;
	int					opt = -1;
	char				*user = NULL;
	char				*passwd = NULL;
	char				*topic = NULL;
	char				*hostname = NULL;
	int					daemon_run = 0;
	int					port;
	char				*program_name;
	char				*test_message = NULL;
	struct trans_info	info;
	char				msg[128];
	int					last = 0,now;
	int					sample_flag = 0;
	float				interval_time = 5;

	program_name = basename(argv[0]);


	struct option long_options[] = 
	{
		{"topic",required_argument,NULL,'t'},
		{"user",required_argument,NULL,'u'},
		{"passwd",required_argument,NULL,'P'},
		{"hostname",required_argument,NULL,'n'},
		{"ip",required_argument,NULL,'i'},
		{"daemon",no_argument,NULL,'d'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc,argv,"u:p:t:n:i:dp:hm:",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'u':
				user = optarg;
				break;
			case 'P':
				passwd = optarg;
			case 't':
				topic = optarg;
				break;
			case 'n':
				hostname = optarg;
				break;
			case 'i':
				ip = optarg;
				break;
			case 'd':
				daemon_run = 1;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				print_usage(program_name);
				break;
			case 'm':
				test_message = optarg;
				break;
			default:
				break;
		}
	}
//	mosq = &mosq1;
//	mosq->username = user;
//	mosq->password = passwd;
//	mosq->topic = topic;
//	mosq->msgs->store = test_message;

/*  
	if(HOST)
	{
		if((hostnp = gethostbyname(HOST0)) == NULL)
		{
			printf("get host by name failure: %s\n",strerror(errno));
			return -1;
		}
		printf("hostname %s\n",hostnp->h_name);
		ip = inet_ntoa(*(struct in_addr*)hostnp->h_addr);
		printf("ip address is : %s\n",ip);
	}
*/
	//初始化libmosquitto库
	ret = mosquitto_lib_init();
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("init lib error: %s\n",strerror(errno));
		return -1;
	}
	printf("初始化mos库成功\n");

	//创建一个发布端实例
	mosq = mosquitto_new(NULL,true,NULL);

	if(NULL == mosq)
	{
		printf("new pub_test error\n");
		mosquitto_lib_cleanup();
		return -1;
	}
	printf("创建一个发布端实例成功\n");

	//设置回调函数
	mosquitto_connect_callback_set(mosq,my_connect_callback);
	mosquitto_disconnect_callback_set(mosq,my_disconnect_callback);
	mosquitto_publish_callback_set(mosq,my_publish_callback);

	//连接至服务器
	//参数：句柄 ip（host） 端口 心跳
	printf("mosq: %s\nHOST: %s\nPORT: %d\nKEEP_ALIVE: %d\n",mosq,HOST,PORT,KEEP_ALIVE);
	ret = mosquitto_connect(mosq,HOST,PORT,KEEP_ALIVE);

	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("connect broker error: %s\n",strerror(errno));
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		return -1;
	}
	printf("connect broke successfully\n");

	int loop = mosquitto_loop_start(mosq);

	//mosquitto_loop_start作用是开启一个线程，在线程里不停的调用mosquitto_loop()；来处理网络信息
	if(loop != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto loop error\n");
		return -1;
	}
	while(!g_sig_out)
	{
		now = time((time_t *)NULL);
		sample_flag = 0;
		if(interval_time < (now-last))			//it is time to sample
		{
			printf("start sampling!!");
			if(pack_info(&info,msg,sizeof(msg)) < 0)
			{
				printf("sampling failure\n");
			}
			else
			{
				printf("sampling successfully\n");
				sample_flag = 1;
			}
			last = now;
		}
		else
		{
			//publish data
			if(MOSQ_ERR_SUCCESS != (mosquitto_publish(mosq,NULL,"topic1",strlen(msg)+1,msg,0,retain)))
			{
				printf("publish data failure\n");
				retain = 1;
			}
			sleep(2);
			memset(msg,0,sizeof(msg));
		}
		printf("\n\n");

	}
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	printf("end\n");

	return 0;

}
void print_usage(const char *program_name)
{
	printf("the program name is %S\n",program_name);
	printf("you can set parameter in two ways\n");
	printf("<1> %s -i<server_ip> -p<server_port> -h< see more help>\n",program_name);
	printf("<2> %s -n<server_hostname> -p<server_port> -h<see more help>\n",program_name);
	printf("	-i	--ip		the ip address of the server you want to connect\n");
	printf("	-n	--hostname	the hostname of the server you want to connect\n");
	printf("	-p	--port		the port of the server you want to connect\n");
	printf("	-d	--daemon	the client program running in background\n");
	printf("	-u	--user		the username of the server you want to connect\n");
	printf("	-P	--password	the password of the server you want to connect\n");
	printf("	-m	--test_message this is debug communiacte\n");
	printf("	-h	--help		more detail\n");
}
int pack_info(struct trans_info *info,char *msg,int size)
{
	strncpy(info->sno,NUMBER,sizeof(NUMBER));
	getTime(info->time);
	getTemper(info->temperature);

	memset(msg,0,sizeof(msg));
	snprintf(msg,size,"%s\n%s\n%s",info->sno,info->time,info->temperature);
	printf("\n");
}
void sig_out(int signum)
{
	if(SIGTERM == signum)
	{
		printf("the program is exit\n");
		g_sig_out = 1;
	}
}
