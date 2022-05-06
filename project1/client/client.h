/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(30/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "30/04/22 17:18:49"
 *                 
 ********************************************************************************/

/* 
 *upload data definition
 * */

struct trans_info
{
	char		sno[10];
	char		time[20];
	char		temperature[10];
};


/* 
 *option parmeter definition
 * */

struct option	long_options[] = 
{
	{"ip",required_argument,NULL,'i'},
	{"port",required_argument,NULL,'p'},
	{"sleep_time",required_argument,NULL,'s'},
	{"help",no_argument,NULL,'h'},
	{NULL,0,NULL,0}
};


/*
 *function:prompt parameter input
 * */

static inline void print_usage(char *progname);

/* 
 *function:create socket and connect
 * */

int socket_connect(char *ip,int port);
