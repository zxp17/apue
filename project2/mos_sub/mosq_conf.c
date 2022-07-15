/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mosq_conf.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(12/07/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "12/07/22 17:16:40"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include "iniparser.h"
#include "mosq_conf.h"

int gain_mqtt_conf(char *ini_path,st_mqtt *mqtt,int type)
{
	dictionary		*ini=NULL;
	const char		*hostname;
	const char		*username;
	int				port;
	const char		*passwd;
	const char		*clientid;
	const char		*topic;
	int				Qos;

	const char		*method;
	const char		*jsonid;
	const char		*identifier;
	const char		*version;

	if(!ini_path || !mqtt)
	{
		printf("invalid input parameter in %s\n",__FUNCTION__);
		return -1;
	}

	ini = iniparser_load(ini_path);
	if(ini == NULL)
	{
		printf("inipar_load failure\n");
		return -1;
	}

	//先默认配置文件内的参数
	hostname	=iniparser_getstring(ini,"mqtt_server_addr:host",DEFAULT_HOSTNAME);
	port		=iniparser_getint(ini,"mqtt_server_addr:port",DEFAULT_PORT);
	username	=iniparser_getstring(ini,"user_passwd:username",DEFAULT_USERNAME);
	passwd		=iniparser_getstring(ini,"user_passwd:passwd",DEFAULT_PASSWD);
	clientid	=iniparser_getstring(ini,"client_id:id",DEFAULT_CLIENTID);
	identifier	=iniparser_getstring(ini,"ali_json:identifier",DEFAULT_IDENTIFIER);
	Qos			=iniparser_getint(ini,"ali_Qos:Qos",DEFAULT_QOS);

	if(type == SUB)
	{
		topic = iniparser_getstring(ini,"sub_topic: topic",DEFAULT_SUBTOPIC);
	}
	else if(type == PUB)
	{
		topic = iniparser_getstring(ini,"pub_topic:topic",DEFAULT_PUBTOPIC);
		method = iniparser_getstring(ini,"json:method",DEFAULT_METHOD);
		jsonid = iniparser_getstring(ini,"json:id",DEFAULT_JSONID);
		version = iniparser_getstring(ini,"json:version",DEFAULT_VERSION);
	}

	mqtt->Qos = Qos;
	strncpy(mqtt->hostname,hostname,BUF_SIZE);
	mqtt->port = port;
	strncpy(mqtt->username,username,BUF_SIZE);
	strncpy(mqtt->passwd,passwd,BUF_SIZE);
	strncpy(mqtt->clientid,clientid,BUF_SIZE);
	strncpy(mqtt->topic,topic,BUF_SIZE);

	if(type == PUB)
	{
		strncpy(mqtt->method,method,BUF_SIZE);
		strncpy(mqtt->identifier,identifier,BUF_SIZE);
		strncpy(mqtt->jsonid,jsonid,BUF_SIZE);
		strncpy(mqtt->version,version,BUF_SIZE);
	}
	iniparser_freedict(ini);

	return 0;
}
