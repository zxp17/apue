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

 void getTemper(char  *s)
{
	int			fd = -1;
	char		buf[128];
	char		*ptr = NULL;
	double		temp = 0;
	int			sig = 25;
//	char		final[128] = {0};
//	char		*final = NULL;

	fd = open("/sys/bus/w1/devices/28-0317320a8aff/w1_slave",O_RDONLY);
	if(fd < 0)
	{
		printf("open file failure: %s\n",strerror(errno));
		perror("open file failure");
	}

	memset(buf,0,sizeof(buf));
	read(fd,buf,sizeof(buf));
//	printf("buf: %s\n",buf);

	ptr = strstr(buf,"t=");
	if(!ptr)
	{
		printf("can not find t = string\n");
	}
	
	ptr += 2;
//	printf("(我是字符串温度值)ptr:%s\n",ptr);
	temp = atof(ptr);
//	printf("(我是浮点型温度值）temp/1000: %f\n",temp/1000);


	gcvt((int)(temp/1000),sig,s);


//	printf("s: %s\n",s);

//	*s = final;
//	printf("s = %p\n",s);


	close(fd);

//	printf("获取温度函数执行完毕\n");

	
}
