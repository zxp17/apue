/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(19/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "19/04/22 17:48:36"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include "client.h"
#include "gettime.h"
#include "gettemper.h"

#define	EQUIP_NUMBER	"Raspberrypi007"
//#define DEBUG


int main(int argc,char **argv)
{
	int					conn_fd = -1;
	int					rv = -1;
	char				buf[1024];
	struct sockaddr_in	serv_addr;
	int					serv_port;
	char				*serv_ip = NULL;
	int					opt;
	char				*progname = NULL;
	char				info[2048];
	char				character[1] = "\n";
	int					sleep_time;
	static int			g_disconnect = 0;
	struct timeval      before,after;
	struct trans_info		tt;

	/*
	 *command ling parameter parsing
	 * */
	while((opt = getopt_long(argc,argv,"i:p:s:h",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'i':
				serv_ip = optarg;
				break;
			case 'p':
				serv_port = atoi(optarg);
				break;
			case 'h':		//get help
				print_usage(progname);
				return EXIT_SUCCESS;
			case 's':
				sleep_time = atoi(optarg);
			default:
				break;
		}
	}

	conn_fd = socket_connect(serv_ip,serv_port);

	while(1)
	{
		gettimeofday(&before,NULL);
#ifdef DEBUG		
		printf("seconds of before: %ld\n",before.tv_sec);
#endif
		/* 
		 *upload data assignment
		 * */
		strncpy(tt.equipment_number,"DB18B20",sizeof("DB18B20"));
		getTemper(tt.temperature);
		getTime(tt.time);
#ifdef DEBUG
		printf("tt.equipment_number: %s\n",tt.equipment_number);
		printf("tt.time: %s\n",tt.time);
		printf("tt.temperature: %s\n",tt.temperature);
		printf("\n");
#endif
		/*
		 *upload data splicing
		 * */
	
		memset(info,0,sizeof(info));
		strncat(info,tt.equipment_number,strlen(tt.equipment_number));
		strncat(info,character,strlen(character)+1);
		strncat(info,tt.time,strlen(tt.time));
		strncat(info,character,strlen(character)+1);
		strncat(info,tt.temperature,strlen(tt.temperature));
		/* 
		 *drop the line and reconnect
		 * */


		if(g_disconnect)
		{
			conn_fd = socket_connect(serv_ip,serv_port);
			g_disconnect = 0;
		}
		if(write(conn_fd,info,strlen(info)) < 0)
		{
			printf("write equipment_number to server [%s:%d] failure: %s\n",serv_ip,serv_port,strerror(errno));
			goto cleanup;
		}
	
		printf("\n\nwrite data to server [%s:%d] successfully\ninfo is : %s\n",serv_ip,serv_port,info);
	

		memset(buf,0,sizeof(buf));
		rv = read(conn_fd,buf,sizeof(buf));
		if(rv < 0)
		{
			printf("read data from server failure: %s\n",strerror(errno));
			goto cleanup;
		}
		else if(0 == rv)
		{
			printf("client connect to server get disconnect\n");
			g_disconnect = 1;
		}
#ifdef DEBUG		
		printf("read %d bytes data from server: '%s'\n",rv,buf);
#endif
		gettimeofday(&after,NULL);

		while(sleep_time > (after.tv_sec - before.tv_sec))
		{
			gettimeofday(&after,NULL);
			continue;
		}
#ifdef DEBUG		
		printf("seconds of after: %ld\n",after.tv_sec);
#endif

	}
cleanup:
	close(conn_fd);

}

static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTION]\n",progname);

	printf("%s is a socket server program,which used to verify client and echo back string from it\n");
	printf("\nmandatory arguments to long options are mandatory for short options too\n");

	printf("-i[ip ] set ip address\n");
	printf("-p[port ]socket server port address\n");
	printf("-h[help ] display information");
	printf("-s[sleep_time ] set interval time");

	printf("\n example:%s -i 127.0.0.1 -p 7777 -s 2 \n",progname);
	return ;
}

int socket_connect(char *ip,int port)
{

	struct sockaddr_in		serv_addr;

	int connfd = socket(AF_INET,SOCK_STREAM,0);

	if(connfd < 0)
	{
		printf("create socket failure: %d\n",strerror(errno));
		return -1;
	}
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_aton(ip,&serv_addr.sin_addr);

	if(connect(connfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("connect to server [%s:%d] failure: %s\n",ip,port,strerror(errno));
		return -1;
	}
	return connfd;
}
