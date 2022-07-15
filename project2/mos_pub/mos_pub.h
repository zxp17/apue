/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mos_pub.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(11/07/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "11/07/22 18:58:58"
 *                 
********************************************************************************/
#ifndef MOS_PUB_H
#define MOS_PUB_H

#include "mosq_conf.h"

struct trans_info
{
	char			sno[20];
	char			time[20];
	char			temperature[20];
};

void print_usage(const char *program_name);
void sig_out(int signum);
int pack_info(struct trans_info *info,char *msg,int size);
void pub_json_data(struct mosquitto *mosq,st_mqtt *mqt);
#endif
