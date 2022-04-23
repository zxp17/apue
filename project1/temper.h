/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  temper.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(19/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "19/04/22 19:48:59"
 *                 
 ********************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

float getTemper()
{
	int			fd = -1;
	char		buf[128];
	char		*ptr = NULL;
	double		temp = 0;
	char		str[128] = {0};
	int			sig = 25;

	fd = open("/sys/bus/w1/devices/28-0317320a8aff/w1_slave",O_RDONLY);
	if(fd < 0)
	{
		printf("open file failure: %s\n",strerror(errno));
		perror("open file failure");
		return -1;
	}

	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));
	printf("buf: %s\n",buf);

	ptr = strstr(buf,"t=");
	if(!ptr)
	{
		printf("can not find t = string\n");
		return -1;
	}
	
	ptr += 2;
	printf("ptr:%s\n",ptr);
	temp = atof(ptr);
	printf("temp: %f\n",temp/1000);

//	gcvt(temp/1000,sig,str);
//	printf("str: %s\n",str);

	close(fd);

	return temp/1000;

	printf("获取温度函数执行完毕\n");
}
