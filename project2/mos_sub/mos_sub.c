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
#include "mos_sub.h"
#include "libgpiod-led.h"

#define		INI_PATH		"./mosq_conf.ini"

#define		HOST			"localhost"
#define		PORT			1883
#define		KEEP_ALIVE		60
#define		MSG_MAX_SIZE	512

static int running = 1;

void my_connect_callback(struct mosquitto *mosq,void *obj,int rc)
{

	printf("callback: connect broke successfully\n");
	if(rc)
	{
		//connect error
		printf("connect broke error: %s\n",strerror(errno));
	}
	else
	{
		//connect successfully,subscribe topic
		if(mosquitto_subscribe(mosq,NULL,"/sys/hh80M2NC5KC/led_/thing/service/property/set",0))
		{
			printf("set the topic error\n");
			exit(1);
		}
		printf("subscribe successfully\n");
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
	char	*status = NULL;
	int		led_flag = 0;
	printf("call the function: on_message\n");
	printf("recieve a message of %s\n: %s\n",(char *)msg->topic,(char *)msg->payload);
	
	status = getStatus((char *)msg->payload);

	led_flag = atoi(status);
	printf("led_flag in message: %d\n",led_flag);

	if(controlLED(led_flag) != 0)
	{
		printf("control error\n");
	}
/*
	if(0 == strcmp(msg->payload,"quit"))
	{
		mosquitto_disconnect(mosq);
	}
*/	
}


int main(int argc,char *argv[])
{
	int					ret;
	struct mosquitto	*mosq;
	int					opt;
	char				*program_name;
	int					port;
	int					daemon_run = 0;
	char				*ip = NULL;
	st_mqtt				mqtt;
	int					rv;
	const char			*sub;

	memset(&mqtt,0,sizeof(mqtt));
	rv = gain_mqtt_conf(INI_PATH,&mqtt);

	struct option long_option[] = 
	{
		{"daemon",no_argument,NULL,'d'},
		{"help",no_argument,NULL,'d'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc,argv,"dh",long_option,NULL)) != -1)
	{
		switch(opt)
		{
			case 'd':
				daemon_run = 1;
				break;
			case 'h':
				print_usage(program_name);
				break;
			default:
				break;
		}
	}


	program_name = basename(argv[0]);

	//init mosquitto
	ret = mosquitto_lib_init();
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("init lib error\n");
		return -1;
	}
	printf("init mosquitto successfully\n");


	//create a mosq
	mosq = mosquitto_new(mqtt.clientid,true,(void *)&mqtt);
	if(NULL == mosq)
	{
		printf("new sub_test error\n");
		goto cleanup;
	}
	printf("mosquitto new successfully\n");


	//set callback function
	mosquitto_connect_callback_set(mosq,my_connect_callback);
	mosquitto_disconnect_callback_set(mosq,my_disconnect_callback);
	mosquitto_subscribe_callback_set(mosq,my_subscribe_callback);
	mosquitto_message_callback_set(mosq,my_message_callback);


	//set username and passwd
	ret = mosquitto_username_pw_set(mosq,mqtt.username,mqtt.passwd);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_username_pw_set failure: %s\n",strerror(errno));
		goto cleanup;
	}

	//connect broke
	ret = mosquitto_connect(mosq,mqtt.host,mqtt.port,KEEP_ALIVE);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		printf("connect server error: %s\n",strerror(errno));
		goto cleanup;
	}

	printf("start communicate~~~~~~~~~~\n");
	while(running)
	{
		mosquitto_loop(mosq,-1,1);
		printf("\n\n");
		sleep(3);
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
char* getStatus(char *msg)
{
	char	*status = NULL;
	//printf("msg in getStatus is: %s\n",msg);
	
	status = strstr(msg,"LEDSwitch");
	if(!status)
	{
		printf("ptr has problem\n");
	}
	status += 11;
	//printf("statuus in getStatus is: %s\n",status);
	
	return (char*)status;
}
