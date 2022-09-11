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
#include "libgpiod-led.h"
#include "pcf8591_smoke.h"
#include "logger.h"

#define		INI_PATH		"./mosq_conf.ini"

#define		KEEP_ALIVE		60
#define		MSG_MAX_SIZE	512
#define		CONNECT			1
#define		DISCONNECT		0

static int running = 1;
static int g_sig_out = 0;
static int connect_flag = 1;
static int disconnect_flag = 0;

void connect_callback(struct mosquitto *mosq,void *obj,int rc)
{
	st_mqtt				*mqtt = obj;

	mqtt->status	= connect_flag;
	if(rc)
	{
		//connect error
		log_error("connect broke error: %s\n",strerror(errno));
	}
	else
	{
		if(mosquitto_subscribe(mosq,NULL,"/sys/hh80SkkdSUQ/mos_pub/thing/service/property/set",0))
		{
			log_error("set the subscribe topic failure\n");
			exit(1);
		}
		log_info("subscribe successfully\n");
	}
}

void disconnect_callback(struct mosquitto *mosq,void *obj,int rc)
{
	st_mqtt				*mqtt = obj;

	mqtt->status = disconnect_flag;
	log_info("callback: disconnected!!\n");
	running = 0;
}
void subscribe_callback(struct mosquitto *mosq,void *obj,int mid,int qos_count,const int *granted_qos)
{
	log_info("callback from subscribe\n");
}
void message_callback(struct mosquitto *mosq,void *obj,const struct mosquitto_message *msg)
{
	char		*status = NULL;
	int			led_flag = 0;
	log_info("call the function: on message\n");
	log_info("recieve a message of %s\n: %s\n",(char *)msg->topic,(char *)msg->payload);

	status = getStatus((char *)msg->payload);

	led_flag = atoi(status);
	log_info("led_flag in message: %d\n",led_flag);

	if(controlLED(led_flag) != 0)
	{
		log_info("control error\n");
	}
}

void publish_callback(struct mosquitto *mosq,void *obj,int mid)
{
	log_info("callback: publish called!!\n");
}


int main(int argc,char *argv[])
{
	int					ret;
	struct mosquitto	*mosq;
	int					opt = -1;
	int					daemon_run = 0;
	char				*program_name;
	sqlite3				*db;
	char				msg[512]; 
	st_mqtt				mqtt;
	int					rv;
	int					platform = 0;
	int					interval_time;
	int					last = 0,now;
	char				*s_data = NULL;
	int					sample_flag = 0;

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

	while((opt = getopt_long(argc,argv,"dhawti:",long_options,NULL)) != -1)
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
				platform = ALI;
				break;
			case 'w':
				platform = HUAWEI;
				break;
			case 't':
				platform = TENCENT;
				break;
			case 'i':
				interval_time = atoi(optarg);
			default:
				break;
		}
	}

	rv = gain_mqtt_conf(INI_PATH,&mqtt, platform);
	if(rv != 0)
	{
		log_error("ali get conf failed\n");
		return -1;
	}

	if((open_database("pub.db",&db))<0)
	{
		log_error("open database failure\n");
		return -1;
	}

	//init mosquitto lib
	ret = mosquitto_lib_init();
	if(ret != MOSQ_ERR_SUCCESS)
	{
		log_error("init lib error: %s\n",strerror(errno));
		return -1;
	}
	log_info("init mosquitto lib successfully\n");


	//mosquitto_new
	mosq = mosquitto_new(mqtt.clientid,true,(void *)&mqtt);
	if(NULL == mosq)
	{
		log_error("new pub_test error\n");
		goto cleanup;
	}
	log_info("create a client successfully\n");

	if(logger_init("mos_pub.log",LOG_LEVEL_DEBUG) < 0)
	{
         fprintf(stderr,"initial logger system failure\n");
         return -1;
	}
	

	//set callback function
	mosquitto_connect_callback_set(mosq,connect_callback);
	mosquitto_disconnect_callback_set(mosq,disconnect_callback);
	mosquitto_publish_callback_set(mosq,publish_callback);
	mosquitto_subscribe_callback_set(mosq,subscribe_callback);
	mosquitto_message_callback_set(mosq,message_callback);


	if(mosquitto_username_pw_set(mosq,mqtt.username,mqtt.passwd) != MOSQ_ERR_SUCCESS)
	{
		log_error("mosquitto_username_pw_set failure: %s\n",strerror(errno));
		goto cleanup;
	}
	log_info("mosquitto_username_pw_set successfully\n");


	//connect broke
	ret = mosquitto_connect(mosq,mqtt.host,mqtt.port,KEEP_ALIVE);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		log_error("connect broker error: %s\n",strerror(errno));
		goto cleanup;
	}
	log_info("connect broke successfully\n");
	

	mosquitto_connect_callback_set(mosq,connect_callback);
	
	if(daemon_run)
	{
		daemon(1,1);
	}


	while(!g_sig_out)
	{
		//data is reported in json format
		//mosquitto_loop(mosq,-1,1);
		now = time((time_t *)NULL);
		sample_flag = 0;

		if(interval_time <= (now - last))
		{
			log_info("start sampling\n");
			if(json_data(mosq,&mqtt,msg) != 0)
			{
				log_info("sample data failure\n");
				continue;
			}
			else
			{
				log_info("msg: %s\n",msg);
				log_info("sample data successfully\n");
				sample_flag = 1;
			}
			last = now;
		}	
		if(disconnect_flag)
		{
			log_info("client disconnect\n");

		}
		else
		{
			sleep(3);
			ret = mosquitto_publish(mosq,NULL,mqtt.topic,strlen(msg)+1,msg,mqtt.Qos,NULL);
			if(ret != MOSQ_ERR_SUCCESS)
			{
				log_info("mosquitto publish failure\n");

				if(save_database(db,msg) < 0)
				{
					log_info("save data failue\n");
				}
				else
				{
					log_info("save data successfully\n");
				}
			}
			if(select_database(db,s_data,sizeof(s_data)) > 0)
			{
				if(mosquitto_publish(mosq,NULL,mqtt.topic,strlen(msg)+1,msg,mqtt.Qos,NULL) != MOSQ_ERR_SUCCESS)
				{
					log_error("publish data in database failure\n");
				}
				else
				{
					if(delete_database(db) < 0)
					{
						log_error("delete data in database failure\n");
					}
				}
			}	
			else
			{
				log_info("database is empty\n");
			}
		}

		mosquitto_loop(mosq,-1,1);
		printf("\n\n");
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
		log_info("the program is exit\n");
		g_sig_out = 1;
	}
}

int json_data(struct mosquitto *mosq,st_mqtt *mqt,char *payload)
{
	char		*msg;
	float		tem = 0;
	char		tim[25];
	float		smokescope = 0;
	char		trans_data[50];


	cJSON	*root = cJSON_CreateObject();
	cJSON	*item = cJSON_CreateObject();

	memset(root,0,sizeof(root));
	memset(item,0,sizeof(item));

	
	getTime(tim);
	getTemper(&tem);
	getSmokescope(&smokescope);

	snprintf(trans_data,sizeof(trans_data),"%d.%d",(int)smokescope,(int)tem);
//	printf("trans_data: %s\n",trans_data);

//	printf("smokescope in mos_pub: %f",smokescope);


//	snprintf(buf,sizeof(buf),"%s/%f",tim,tem);
	
	cJSON_AddItemToObject(root,"method",cJSON_CreateString(mqt->method));
	cJSON_AddItemToObject(root,"id",cJSON_CreateString(mqt->jsonid));
	cJSON_AddItemToObject(root,"params",item);
	cJSON_AddItemToObject(root,"time",cJSON_CreateString(tim));
	cJSON_AddItemToObject(item,"Temperature",cJSON_CreateNumber(smokescope));
	cJSON_AddItemToObject(root,"version",cJSON_CreateString(mqt->version));

	
	msg = cJSON_Print(root);
	snprintf(payload,512,"%s",msg);

	return 0;

}
char *getStatus(char *msg)
{
	char	*status = NULL;
	log_debug("msg in getStatus is: %s\n",msg);

	status = strstr(msg,"Temperature");
	if(!status)
	{
		log_debug("ptr has problem\n");
	}
	status += 13;
	log_debug("status in getStatus is: %s\n",status);

	return (char*)status;
}
