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

/* 
 *function:print prompt information
 * */
void print_usage(const char *program_name);

/* 
 *function:catch program exit signal
 * */
void sig_out(int signum);

/* 
 *function:the information is reported in JSON format
 * */
void pub_json_data(struct mosquitto *mosq,st_mqtt *mqt);


#endif
