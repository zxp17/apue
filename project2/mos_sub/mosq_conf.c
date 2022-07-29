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

int gain_mqtt_conf(char *ini_path,st_mqtt *mqtt)
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
	mqtt->port = iniparser_getint(ini,"ali:port",-1);
	mqtt->Qos = iniparser_getint(ini,"json:Qos",-1);

	strncpy(mqtt->host,iniparser_getstring(ini,"ali:host","NULL"),BUF_SIZE);
	strcpy(mqtt->username,iniparser_getstring(ini,"ali:username","NULL"));
	strncpy(mqtt->passwd,iniparser_getstring(ini,"ali:passwd","NULL"),BUF_SIZE);
	strncpy(mqtt->clientid,iniparser_getstring(ini,"ali:id","NULL"),BUF_SIZE);
	strncpy(mqtt->topic,iniparser_getstring(ini,"ali:topic","NULL"),BUF_SIZE);

	strncpy(mqtt->method,iniparser_getstring(ini,"json:method","NULL"),BUF_SIZE);
	strncpy(mqtt->jsonid,iniparser_getstring(ini,"json:id","NULL"),BUF_SIZE);
	strncpy(mqtt->identifier,iniparser_getstring(ini,"json:identifier","NULL"),BUF_SIZE);
	strncpy(mqtt->version,iniparser_getstring(ini,"json:version","NULL"),BUF_SIZE);


	printf("mqtt->username in conf: %s\n",mqtt->username);

	iniparser_freedict(ini);

	return 0;
}
