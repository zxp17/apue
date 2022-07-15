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
#include <errno.h>
#include "mos_pub.h"
#include "gettime.c"
#include "gettemper.c"
#include "client_database.c"
#include "mosq_conf.h"
#include "cJSON.h"
#include "dictionary.h"
#include "iniparser.h"

#define		INI_PATH		"./mosq_conf_ini"

#define		HOST			"localhost"
#define		PORT			1883
#define		KEEP_ALIVE		60
#define		MSG_MAX_SIZE	512



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


int main(int argc,char *argv[])
{
	int					ret;
	struct mosquitto	*mosq;
	int					retain = 0;
	int					opt = -1;
	int					daemon_run = 0;
	char				*program_name;
	char				*test_message = NULL;
	char				msg[128];
	st_mqtt				mqtt;
	int					rv;
	int					ali = 0;
	int					huawei = 0;
	int					tencent = 0;

	//init mqtt
	memset(&mqtt,0,sizeof(mqtt));
	rv = gain_mqtt_conf(INI_PATH,&mqtt,TENCENT);

	program_name = basename(argv[0]);

	struct option long_options[] = 
	{
		{"daemon",no_argument,NULL,'d'},
		{"help",no_argument,NULL,'h'},
		{"ali",no_argument,NULL,'a'},
		{"huawei",no_argument,NULL,'w'},
		{"tencent",no_argument,NULL,'t'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc,argv,"dhaht",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'd':
				daemon_run = 1;
				break;
			case 'h':
				print_usage(program_name);
				break;
			case 'a':
				ali = 1;
				break;
			case 'w':
				huawei = 1;
				break;
			case 't':
				tencent = 1;
				break;
			default:
				break;
		}
	}
	//init mosquitto lib
	ret = mosquitto_lib_init();
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("init lib error: %s\n",strerror(errno));
		return -1;
	}
	printf("init mosquitto lib successfully\n");

	//mosquitto_new
	mosq = mosquitto_new(mqtt.clientid,true,(void *)&mqtt);
	if(NULL == mosq)
	{
		printf("new pub_test error\n");
		goto cleanup;
	}
	printf("create a client successfully\n");

	//set callback function
	mosquitto_connect_callback_set(mosq,my_connect_callback);
	mosquitto_disconnect_callback_set(mosq,my_disconnect_callback);
	mosquitto_publish_callback_set(mosq,my_publish_callback);

	if(mosquitto_username_pw_set(mosq,mqtt.username,mqtt.passwd) != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_username_pw_set failure: %s\n",strerror(errno));
		goto cleanup;
	}
	printf("mosquitto_username_pw_set successfully\n");

	
	printf("mosq: %s\nhostname: %s\nport: %d\nKEEP_ALIVE: %d\n",mosq,mqtt.hostname,mqtt.port,KEEP_ALIVE);

	//connect broke
	ret = mosquitto_connect(mosq,mqtt.hostname,mqtt.port,KEEP_ALIVE);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("connect broker error: %s\n",strerror(errno));
		goto cleanup;
	}
	printf("connect broke successfully\n");


	while(!g_sig_out)
	{
		//发送格式为cJSON的上报数据
		pub_json_data(mosq,&mqtt);
	}
cleanup:	
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	printf("end\n");

	return 0;

}
void print_usage(const char *program_name)
{
	printf("the program name is %S\n",program_name);
	printf("	-d	--daemon	the client program running in background\n");
	printf("	-h	--help		more detail\n");
}

void sig_out(int signum)
{
	if(SIGTERM == signum)
	{
		printf("the program is exit\n");
		g_sig_out = 1;
	}
}

void pub_json_data(struct mosquitto *mosq,st_mqtt *mqt)
{
	char		buf[512];
	float		tem = 0;
	char		tim[32];
	char		*msg;

	cJSON	*root = cJSON_CreateObject();
	cJSON	*item = cJSON_CreateObject();

	memset(root,0,sizeof(root));
	memset(item,0,sizeof(item));

/*	if(getTime(tim) < 0)
	{
		printf("get time failure: %s\n",strerror(errno));
		return ;
	}
	if(getTemper(tem) < 0)
	{
		printf("get temperature failure: %s\n",strerror(errno));
		return ;
	}
*/
	getTime(tim);
	getTemper(&tem);

	snprintf(buf,sizeof(buf),"%s/%f",tim,tem);

	cJSON_AddItemToObject(root,"method",cJSON_CreateString(mqt->method));
	cJSON_AddItemToObject(root,"id",cJSON_CreateString(mqt->jsonid));
	cJSON_AddItemToObject(root,"params",item);
	cJSON_AddItemToObject(root,"time",cJSON_CreateString(tim));
	cJSON_AddItemToObject(item,"CurrentTemperature",cJSON_CreateNumber(tem));
	cJSON_AddItemToObject(root,"version",cJSON_CreateString(mqt->version));

	msg = cJSON_Print(root);
	printf("msg: %s\n",msg);

	if(mosquitto_publish(mosq,NULL,mqt->topic,strlen(msg)+1,msg,mqt->Qos,NULL) != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto publish failure: %s\n",strerror(errno));
		return ;
	}
	printf("mosquitto_publish successfully\n");
}
