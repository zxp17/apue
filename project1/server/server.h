/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  server.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(30/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "30/04/22 17:37:30"
 *                 
 ********************************************************************************/

/*
 *command ling parameter definition
 * */

struct option		long_options[] = 
{
	{"daemon",no_argument,NULL,'d'},
	{"port",required_argument,NULL,'p'},
	{"help",no_argument,NULL,'h'},
	{NULL,0,NULL,0},
};


/*
 *initalization of a socket
 * */
int socket_server_init(char *listen_ip,int listen_port);


/* 
 *command line parameter input prompt
 * */
static inline void print_usage(char *progname);


/* 
 *sets the maximun number of file descriptors
 * */
void set_socket_rlimit(void);
