/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  gettime.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "21/04/22 15:26:47"
 *                 
 ********************************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/* 
 *get time function
 * */ 
void getTime(char *s)
{
	time_t 				timep;
	struct tm			*p;
	

	time(&timep);
	p = gmtime(&timep);


	snprintf(s,30,"%d/%d/%d %d:%d:%d",1900+p->tm_year,1+p->tm_mon,p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec);


#ifdef DEBUG
	printf("获取时间函数执行完毕\n");
#endif
}
