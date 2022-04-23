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

char* getTime()
{
	time_t 		*timep = malloc(sizeof(*timep));

	time(timep);
	char		*s = ctime(timep);

	printf("%s",s);

	return s;
}
