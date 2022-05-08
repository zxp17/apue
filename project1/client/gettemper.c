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

/* 
 *get temperature function
 * */
 void getTemper(char  *s)
{
	int			fd = -1;
	char		buf[128];
	char		*ptr = NULL;
	double		temperature = 0;
	int			sig = 25;


	fd = open("/sys/bus/w1/devices/28-0317320a8aff/w1_slave",O_RDONLY);
	if(fd < 0)
	{
		printf("open file failure: %s\n",strerror(errno));
		perror("open file failure");
	}

	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));

	ptr = strstr(buf,"t=");
	if(!ptr)
	{
		printf("can not find t = string\n");
	}	

	ptr += 2;
	temperature = atof(ptr);

	gcvt((temperature/1000),sig,s);

	close(fd);

#ifdef DEBUG	
	printf("获取温度函数执行完毕\n");
#endif
	
}
