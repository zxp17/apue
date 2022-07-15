/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mos_sub.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/05/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "16/05/22 19:08:05"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mosquitto.h>
#include <libgen.h>
#include <getopt.h>
#include "iniparser.h"
#include "cJSON.h"
#include "dictionary.h"
#include "mosq_conf.h"

#define		INI_PATH		"./mosq_conf_ini"

#define		HOST			"localhost"
#define		PORT			1883
#define		KEEP_ALIVE		60
#define		MSG_MAX_SIZE	512

//定义运行标志决定是否需要结束
static int running = 1;

void my_connect_callback(struct mosquitto *mosq,void *obj,int rc)
{
	
	printf("callback: connect broke successfully\n");
	printf("rc: %d\n",rc);

	if(rc)
	{
		//连接错误，退出程序
		printf("connect broke error: %s\n",strerror(errno));
	}
	else
	{
		//订阅主题
		//参数：句柄：id 订阅的主题：qos
		if(mosquitto_subscribe(mosq,NULL,"topic1",2))
		{
			printf("set the topic error\n");
			exit(1);
		}
	}
}
void my_disconnect_callback(struct mosquitto *mosq,void *obj,int rc)
{
	printf("callback: disconnected!!\n");
	running = 0;
}

void my_subscribe_callback(struct mosquitto *mosq,void *obj,int mid,int qos_count,const int *granted_qos)
{
	printf("callback subscribe successfully\n");
}


void my_message_callback(struct mosquitto *mosq,void *obj,const struct mosquitto_message *msg)
{
	printf("call the function: on_message\n");
	printf("recieve a message of %s: %s\n",(char *)msg->topic,(char *)msg->payload);

	if(0 == strcmp(msg->payload,"quit"))
	{
		mosquitto_disconnect(mosq);
	}
}


int main(int argc,char *argv[])
{
	int					ret;
	struct mosquitto	*mosq;
	int					opt;
	char				*program_name;
	char				*user = NULL;
	char				*passwd = NULL;
	char				*topic = NULL;
	char				*hostname = NULL;
	int					port;
	int					daemon_run = 0;
	char				*ip = NULL;
	st_mqtt				mqtt;
	int					rv;

	memset(&mqtt,0,sizeof(mqtt));
	rv = gain_mqtt_conf(INI_PATH,&mqtt,SUB);

	struct option long_option[] = 
	{
		{"topic",required_argument,NULL,'t'},
		{"user",required_argument,NULL,'u'},
		{"passwd",required_argument,NULL,'P'},
		{"hostname",required_argument,NULL,'n'},
		{"ip",required_argument,NULL,'i'},
		{"daemon",no_argument,NULL,'d'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'d'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc,argv,"u:P:t:n:i:dp:h",long_option,NULL)) != -1)
	{
		switch(opt)
		{
			case 'u':
				user = optarg;
				break;
			case 'P':
				passwd = optarg;
				break;
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
			default:
				break;
		}
	}


	program_name = basename(argv[0]);
	//初始化mosquitto库
	ret = mosquitto_lib_init();
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("init lib error\n");
		return -1;
	}
	printf("初始化mos库成功\n");
	//创建一个订阅端实例
	//参数：id（不需要则为NULL） clean_start 用户数据
	mosq = mosquitto_new("mqtt.clientid",true,(void *)&mqtt);
	if(NULL == mosq)
	{
		printf("new sub_test error\n");
		mosquitto_lib_cleanup();
		return -1;
	}
	printf("mosquitto new successfully\n");

	//set callback function
	mosquitto_connect_callback_set(mosq,my_connect_callback);
	mosquitto_disconnect_callback_set(mosq,my_disconnect_callback);
	mosquitto_subscribe_callback_set(mosq,my_subscribe_callback);
	mosquitto_message_callback_set(mosq,my_message_callback);

	//set username and passwd
	if(mosquitto_username_pw_set(mosq,mqtt.username,mqtt.passwd) != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_username_pw_set failure: %s\n",strerror(errno));
		mosquitto_lib_cleanup();
	}

	//connect broke
	ret = mosquitto_connect(mosq,mqtt.hostname,mqtt.port,KEEP_ALIVE);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("connect server error: %s\n",strerror(errno));
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		return -1;
	}
	printf("connect broke successfully\n");

	//开始通信，循坏执行，知道运行标志running被改变
	printf("start communicate~~~~~~~~~~\n");

	while(running)
	{
		mosquitto_loop(mosq,-1,1);
		printf("\n\n");
	}

	//结束后的清理工作
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	printf("end\n");

	return 0;
}
void print_usage(const char *program_name)
{
	printf("the program name is %s\n",program_name);
	printf("you can set parameter in two ways\n");
	printf("<1> %s -i<server_ip> -p<server_port> -h<see more help>\n",program_name);
	printf("<2> %s -n<server_hostname> -p<server_port> -h<see more help>\n",program_name);
	printf("	-i	--ip		the ip address of the server\n");
	printf("	-n	--hostname	the hostname of server\n");
	printf("	-p	--port		the port of the server\n");
	printf("	-d	--daemon	the client program running in background\n");
	printf("	-u	--username	the username of server\n");
	printf("	-P	--password	the password of the server\n");
	printf("	-h	--help		more detail\n");
}
