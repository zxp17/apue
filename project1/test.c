/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(30/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "30/04/22 11:14:06"
 *                 
 ********************************************************************************/

#include "create_database.h"

int main()
{
	int 			rc;
	sqlite3			*db;
	char			*sql;
	open_database("temperature_database.db",&db);
	if(rc)
	{
		printf("打开失败\n");
	}
	else
	{
		printf("打开成功\n");
	}
	sql = "INSERT INTO COMPANY (SERIAL,TIME,TEMPERATURE) " \
		   "VALUES ('DBB','SATURDAY','520');";
	execute_exec(db,sql);

	close_database(db);

	return 0;

}
