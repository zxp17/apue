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
#include <sqlite3.h>
#include "mos_pub.h"
#include "gettime.h"
#include "gettemper.h"
#include "client_database.h"
#include "mosq_conf.h"
#include "cJSON.h"
#include "dictionary.h"
#include "iniparser.h"

#define		INI_PATH		"./mosq_conf_ini"

#define		HOST			"localhost"
#define		PORT			1883
#define		KEEP_ALIVE		60
#define		MSG_MAX_SIZE	512


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

	while((opt = getopt_long(argc,argv,"dhawt",long_options,NULL)) != -1)
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
	if(ali)
	{
		rv = gain_mqtt_conf(INI_PATH,&mqtt,ALI);
		if(rv != 0)
		{
			printf("ali get conf failed\n");
			return -1;
		}
	}
	if(huawei)
	{
		rv = gain_mqtt_conf(INI_PATH,&mqtt,HUAWEI);
		if(rv != 0)
		{
			printf("huawei get conf failed\n");
			return -1;
		}
	}
	if(tencent)
	{
		rv = gain_mqtt_conf(INI_PATH,&mqtt,TENCENT);
		if(rv != 0)
		{
			printf("tencen get conf failed\n");
			return -1;
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
	//mosquitto_disconnect_callback_set(mosq,my_disconnect_callback);
	mosquitto_publish_callback_set(mosq,my_publish_callback);

	printf("mqtt.uername: %s,mqtt.passwd: %s",mqtt.username,mqtt.passwd);

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


	if(daemon_run)
	{
		daemon(1,1);
	}


	while(!g_sig_out)
	{
		//data is reported in json format
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
	printf("the program name is %s\n",program_name);
	printf("	-d	--daemon	the client program running in background\n");
	printf("	-h	--help		more detail\n");
	printf("	-a	--ali is connection to ali cloud platform\n");
	printf("	-w	--huawei is connection to huawei cloud platform\n");
	printf("	-t 	--tencent is connection to tencent cloud platform\n");

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
	char		tim[25];
	char		*msg = NULL;
	char		s_data[128];
	sqlite3		*db = NULL;
	int			sample_flag = 0;


	cJSON	*root = cJSON_CreateObject();
	cJSON	*item = cJSON_CreateObject();

	memset(root,0,sizeof(root));
	memset(item,0,sizeof(item));

	
	getTime(tim);
	getTemper(&tem);


//	snprintf(buf,sizeof(buf),"%s/%f",tim,tem);
	
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

		//publish failure save data in database
		printf("mosquitto publish failure: %s\n",strerror(errno));
		if(save_database(db,msg) < 0)
		{
			printf("save data failure\n");
			sample_flag = 1;
		}
		if(sample_flag > 0)
		{
			if(select_database(db,s_data,sizeof(s_data)) > 0)		//database with data
			{
				if(mosquitto_publish(mosq,NULL,mqt->topic,strlen(s_data)+1,s_data,mqt->Qos,NULL) != MOSQ_ERR_SUCCESS)
				{
					printf("publish data in database failure\n");
				}
				else
				{
					printf("publish data in database sucsessfully\n");
					if(delete_database(db) < 0)
					{
						printf("delete data in database failure");
					}
				}
			}
			else
			{
				printf("database is empty\n");
			}
		}
	}
}
