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


#define SERVER_IP		"127.0.0.1"
#define SERVER_PORT		7777
//#define MSG_STR			"hello,unix network program world"

static inline void print_usage(char *progname);

int main(int argc,char **argv)
{
	int					conn_fd = -1;
	int					rv = -1;
	char				buf[1024];
	struct sockaddr_in	serv_addr;
	int					serv_port;
	char				*serv_ip;
	int					opt;
	char				*progname = NULL;
	int					fd = -1;
	char				buf_tem[128];
	char				*ptr = NULL;
	float				tem_f;
	int					sig = 25;
	char				tem_f_s[100];


	struct option		long_options[] = 
	{
		{"ip",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	//命令行参数解析
	while((opt = getopt_long(argc,argv,"i:p:h",long_options,NULL)) != -1)
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
			default:
				break;
				
		}
	}

	//读取树莓派的温度
	fd = open("/sys/bus/w1/devices/28-0317320a8aff/w1_slave",O_RDONLY);
	if(fd < 0)
	{
		printf("open file failure: %s\n",strerror(errno));
		return -1;
	}
	memset(buf_tem,0,sizeof(buf_tem));
	read(fd,buf_tem,sizeof(buf_tem));
	printf("buf_tem: %s\n",buf_tem);

	//通过在字符串中找字串"t="的方式，获取温度值
	ptr = strstr(buf_tem,"t=");

	if(!ptr)
	{
		printf("can not find t = string\n");
		return -1;
	}
	ptr += 2;			//通过输出结果可知，只要将指针后移两位就能获取温度的数值
	printf("ptr: %s\n",ptr);

	tem_f = atof(ptr);
	printf("tem_f: %f\n",tem_f/1000);
	
	gcvt(tem_f/1000,sig,tem_f_s);
	printf("tem_f_s: %s\n",tem_f_s);
	

	close(fd);


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
		if(write(conn_fd,tem_f_s,strlen(tem_f_s)) < 0)
		{
			printf("write data to server [%s:%d] failure: %s\n",serv_ip,serv_port,strerror(errno));
			goto cleanup;
		}

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
		printf("read %d bytes data from server: '%s'\n",rv,buf);
		sleep(100);
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

	printf("\n example:%s -i 127.0.0.1 -p 7777\n",progname);
	return ;
}
