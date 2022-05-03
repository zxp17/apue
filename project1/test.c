/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(02/05/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "02/05/22 14:34:28"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <time.h>

int main()
{
	time_t		timep;
	struct tm	*p;
	char		str[128];

	time(&timep);
	p = gmtime(&timep);

	printf("year: %d\n",1900+p->tm_year);
	printf("month: %d\n",1+p->tm_mon);
	printf("day: %d\n",p->tm_mday);
	printf("hour: %d\n",8+p->tm_hour);
	printf("minute: %d\n",p->tm_min);
	printf("second: %d\n",p->tm_sec);

	snprintf(str,30,"%d/%d/%d/%d/%d/%d",1900+p->tm_year,1+p->tm_mon,p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec);

	printf("str: %s\n",str);

	return 0;
}
