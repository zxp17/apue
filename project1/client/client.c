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
#include "client_database.c"
#include "client.h"
#include "gettime.h"
#include "gettemper.h"

#define	EQUIP_NUMBER	"Raspberrypi007"
#define DEBUG
int pack_info(struct trans_info *info,char *msg,int size);

int main(int argc,char **argv)
{
	int					conn_fd = -1;
	int					rv = -1;
	char				buf[128];
//	struct sockaddr_in	serv_addr;
	int					serv_port;
	char				*serv_ip = NULL;
	int					opt;
	char				*progname = NULL;
	void				*optval;
	int					optlen = sizeof(optval);
	char				msg[128];
	char				character[1] = "\n";
	int					sleep_time;
	struct timeval      before,after;
	struct trans_info	tt;
	sqlite3				*db;

	int					sample_time = 0;
	int					sample_flag = 0;

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
	if((open_database("client_database.db",db))< 0)
	{
		printf("open database failure\n");
		return -1;
		close(conn_fd);
	}

	conn_fd = socket_connect(serv_ip,serv_port);


	while(1)
	{
		gettimeofday(&before,NULL);

		if(sample_time)
		{
			printf("开始采样\n");
			if(pack_info(&tt,msg,sizeof(msg)) < 0)
			{
				printf("sample data failure\n");
				continue;
			}
			else
			{
				printf("sample data successfully\n");
				sample_flag = 1;
			}
			printf("msg : %s\n",msg);
		}
		if(conn_fd < 0)
		{
			conn_fd = socket_connect(serv_ip,serv_port);
			if(conn_fd < 0)
			{
				if(sample_flag)
				{
					if(save_database(db,msg) != 0)
					{
						printf("save data failure\n");
					}
				}
				conn_fd = -1;
			}
			else
			{
				printf("connect server successfully\n");
				conn_fd = 1;
			}
			
		}

		setsockopt(conn_fd,SOL_SOCKET,SO_REUSEADDR,optval,optlen);
		if(optval <= 0)
		{
			if(sample_flag)
			{
				if(save_database(db,msg) != 0)
				{
					printf("save data failure\n");
				}
			}
			conn_fd = -1;
		}
		else				//网络连接正常
		{
			if(conn_fd >= 0)
			{
				if(write(conn_fd,msg,strlen(msg)) < 0)
				{
					printf("send data to server failure\n");
				}
				if(select_database(db,buf) > 0)		//数据库中有残余的数据
				{
					if(write(conn_fd,msg,strlen(msg)) < 0)
					{
						printf("send data to server failure\n");
					}
					else
					{
						printf("send data to server successfull\n");

						if(delete_database(db) != 0)
						{
							printf("delete data failure\n");
						}
						else
						{
							printf("delete data successfully\n");
						}

					}
				}
				else
				{
					printf("the database is empty\n");
				}
			}
		}
		gettimeofday(&after,NULL);
		while(sleep_time > (after.tv_sec - before.tv_sec))
		{
			gettimeofday(&after,NULL);
			continue;
		}
		sample_time = 1;
	}
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
int pack_info(struct trans_info *info,char *msg,int size)
{
	strncpy(info->sno,"rpi007",sizeof("rpi007"));
	getTime(info->time);
	getTemper(info->temperature);

	snprintf(msg,size,"%s\n%s\n%s",info->sno,info->time,info->temperature);

	return 0;

}
