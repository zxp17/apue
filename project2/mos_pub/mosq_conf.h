/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mosq_conf.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(12/07/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "12/07/22 15:53:06"
 *                 
 ********************************************************************************/

#ifndef MQTT_CONF_H
#define MQTT_CONF_H

#define BUF_SIZE 512

/*
#define DEFAULT_CLIENTID		"hh80DrvPW3T.mqtt|securemode=2,signmethod=hmacsha256,timestamp=1657599841531|"
#define DEFAULT_USERNAME		"mqtt&hh80DrvPW3T"
#define DEFAULT_PASSWD			"07a106739ed0a9052dc40677ed201202a57f8395e4b96c4b9b37afd8f4d45e52"
#define DEFAULT_HOSTNAME		"iot-06z00cu7pulrouc.mqtt.iothub.aliyuncs.com"
#define DEFAULT_PORT			1833

#define DEFAULT_SUBTOPIC		"/sys/hh80DrvPW3T/mqtt/thing/service/property/set"	
#define DEFAULT_PUBTOPIC		"/sys/hh80DrvPW3T/mqtt/thing/event/property/post"

#define DEFAULT_QOS				0
#define DEFAULT_METHOD			"aliyun:thing.service.property.set"
#define DEFAULT_JSONID			"xz"					
#define DEFAULT_IDENTIFIER		"CurrentTemperature"
#define DEFAULT_VERSION			"1.0.0.0"
*/


#define KEEP_ALIVE				60

enum{
	ALI,
	HUAWEI,
	TENCENT
};

typedef struct data_st_mqtt{

	char	hostname[BUF_SIZE];
	int		port;
	char	username[BUF_SIZE];
	char	passwd[BUF_SIZE];
	char	clientid[BUF_SIZE];
	int		Qos;

	char	method[BUF_SIZE];
	char	topic[BUF_SIZE];
	char	jsonid[BUF_SIZE];
	char	identifier[BUF_SIZE];
	char	version[BUF_SIZE];
}st_mqtt;

int gain_mqtt_conf(char *ini_path,st_mqtt *mqtt,int type);

#endif
