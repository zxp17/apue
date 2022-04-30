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
#include <fcntl.h>
#include "gettime.h"
#include "gettemper.h"

#define	EQUIP_NUMBER	"DS18B20"


static inline void print_usage(char *progname);

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
	int					sleep_time;


	struct trans_info{

		char			equipment_number[128];
		char			*time;
		char			temperature[128];
	};

	struct trans_info		tt;

	struct option			long_options[] = 
	{
		{"ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"sleep_time",required_argument,NULL,'s'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};


	//命令行参数解析
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


	conn_fd = socket(AF_INET,SOCK_STREAM,0);

	if(conn_fd < 0)
	{
		printf("create socket failure: %d\n",strerror(errno));
		return -1;
	}
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);
	inet_aton(serv_ip,&serv_addr.sin_addr);		


	if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("connect to server [%s:%d] failure: %s\n",serv_ip,serv_port,strerror(errno));
		return 0;
	}
	
	while(1)
	{

		strncpy(tt.equipment_number,"DB18B20",sizeof("DB18B20"));
		getTemper(tt.temperature);
		tt.time = NULL;
		getTime(&tt.time);

		strcat(tt.equipment_number,"\n");
		strcat(tt.equipment_number,tt.time);
		strcat(tt.equipment_number,tt.temperature);
		printf("tt.equipment_number = %s\n",tt.equipment_number);


		if(write(conn_fd,tt.equipment_number,strlen(tt.equipment_number)) < 0)
		{
			printf("write equipment_number to server [%s:%d] failure: %s\n",serv_ip,serv_port,strerror(errno));
			goto cleanup;
		}
		sleep(1);
		printf("\n");

		printf("write successfully\n");

		printf("write data to server [%s:%d] successfully\nequipment_number: %d\ntime: %stemperature: %s\n",serv_ip,serv_port,tt.equipment_number,tt.time,tt.temperature);
		printf("\n\n");

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
			goto cleanup;
		}
		//printf("read %d bytes data from server: '%s'\n",rv,buf);
		sleep(sleep_time);		//修改这部分的处理，通过获取当前时间的差来固定“休眠”的时间
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
