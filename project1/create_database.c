/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  create_database.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "21/04/22 14:31:12"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	sqlite3 	*db;
	char		*zErrMsg = 0;
	int			rc;

	rc = sqlite3_open("temperature_database.db",&db);

	if(rc)
	{
		fprintf(stderr,"can not open database: %s\n",sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr,"opened database successfully\n");
	}
	sqlite3_close(db);
}
