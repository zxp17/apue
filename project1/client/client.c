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
#include <time.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <libgen.h>
#include <netinet/tcp.h>
#include "client_database.h"
#include "client.h"
#include "gettime.h"
#include "gettemper.h"
#include "logger.h"

#define	EQUIP_NUMBER	"Raspberrypi007"
#define DEBUG


int						g_sig_out = 0;

int main(int argc,char **argv)
{
	int					conn_fd = -1;
	int					rv = -1;
	char				s_data[128];
	int					serv_port;
	char				*serv_ip = NULL;
	int					daemon_run = 0;
	int					opt;
	char				*progname = NULL;
	struct tcp_info		on;
	int					optlen = sizeof(on);
	char				msg[128];
	char				character[1] = "\n";
	int					interval_time;
	int					last = 0,now;
	struct trans_info	t_info;
	sqlite3				*db = NULL;

	static int			sample_flag = 0;

	progname = basename(argv[0]);

	/*
	 *command ling parameter parsing
	 * */
	while((opt = getopt_long(argc,argv,"di:p:s:h",long_options,NULL)) != -1)
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
				interval_time = atoi(optarg);
			default:
				break;
		}
	}
	signal(SIGTERM,sig_out);		//register signal

	if(!serv_ip || !serv_port)
	{
		print_usage(progname);
	}
	

	if(daemon_run)
	{
		daemon(1,1);
	}
	
	if(logger_init("client.log",LOG_LEVEL_DEBUG) < 0)
	{
		fprintf(stderr,"initial logger system failure\n");
		return -1;
	}

	
	if((open_database("client_database.db",&db))< 0)
	{
		log_error("open the database failure: %s\n",strerror(errno));
		return -2;
	}


	conn_fd = socket_connect(serv_ip,serv_port);

	while(!g_sig_out)
	{
		now = time((time_t *)NULL);
		sample_flag = 0;

		if(interval_time <= (now - last))
		{

			printf("starting sample\n");
			if(pack_info(&t_info,msg,sizeof(msg)) < 0)
			{
				log_error("sample data failure: %s\n",strerror(errno));
				continue;
			}
			else
			{
				log_info("sample data successfully\n");
				sample_flag = 1;
			}
			last = now;
		}
		if(conn_fd < 0)
		{
			conn_fd = socket_connect(serv_ip,serv_port);
			if(conn_fd < 0)
			{
				log_error("connect server failure: %s\n",strerror(errno));
				sleep(2);
				close(conn_fd);
				conn_fd = -1;
			}
			else
			{
				log_info("connect server successfully\n");
			}
			
		}

		getsockopt(conn_fd,IPPROTO_TCP,TCP_INFO,&on,&optlen);

		if(on.tcpi_state != TCP_ESTABLISHED)
		{
			if(sample_flag)
			{
				if(save_database(db,msg) != 0)
				{
					log_error("save data failure: %s\n",strerror(errno));
				}
			}
			close(conn_fd);
			conn_fd = -1;
		}
		else				//网络连接正常
		{
			if(conn_fd >= 0)
			{
				if(write(conn_fd,msg,strlen(msg)) < 0)
				{
					log_error("send data to server failure: %s\n",strerror(errno));
					if(save_database(db,msg) <  0)
					{
						log_error("save data failure: %s\n",strerror(errno));
						close(conn_fd);
						conn_fd = -1;
					}
				}
				if(select_database(db,s_data,sizeof(s_data)) >= 0)		//数据库中有残余的数据
				{
					if(write(conn_fd,s_data,strlen(s_data)) < 0)
					{
						log_error("send data to server failure: %s\n",strerror(errno));
						close(conn_fd);
						conn_fd = -1;
					}
					else
					{	
						if(delete_database(db) < 0)
						{
							log_error("delete data failure: %s\n",strerror(errno));
						}
					}
				}
				else
				{
					log_info("the database is empty\n");
				}
				sleep(2);
				printf("\n\n");
			}
		}
	}
close(conn_fd);
sqlite3_close(db);
return 0;
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
		log_error("create socket failure: %s\n",strerror(errno));

		return -1;
	}
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_aton(ip,&serv_addr.sin_addr);

	if(connect(connfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
	{
		log_error("connect to server [%s:%d] failure : %s\n",ip,port,strerror(errno));
		return -1;
	}
	return connfd;
}

int pack_info(struct trans_info *info,char *msg,int size)
{
	strncpy(info->sno,EQUIP_NUMBER,sizeof(EQUIP_NUMBER));
	getTime(info->time);
	getTemper(info->temperature);

	memset(msg,0,sizeof(msg));
	snprintf(msg,size,"%s\n%s\n%s",info->sno,info->time,info->temperature);

	log_debug("msg: %S\n",msg);
#ifdef DEBUG
	printf("msg: %s\n",msg);
#endif
	printf("\n");

	return 0;

}
void sig_out(int signum)
{
	if(SIGTERM == signum)
	{
		log_warn("the profram is exit\n");
		g_sig_out = 1;
	}
}
