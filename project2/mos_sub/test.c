/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(29/07/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "29/07/22 14:52:29"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include "iniparser.h"
#include "dictionary.h"

#define	ini_path	"./ini"

int main()
{
	dictionary	*ini = NULL;
	char		info[512];

	ini = iniparser_load(ini_path);

	strncpy(info,iniparser_getstring(ini,"test:info","NULL"),512);
	
	printf("info: %s\n",info);

	return 0;

}
