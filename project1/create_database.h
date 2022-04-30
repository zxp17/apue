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

static int callback(void *NotUsed,int argc,char **argv,char **azColName);
int open_database(const char *filename,sqlite3 **db);
int execute_exec(sqlite3 *db,const char *sql);
int close_database(sqlite3* db);

/*  
sqlite3		*db;
char		*zErrMsg = 0;
int			rc;
char		*sql;
*/

//打开数据库
int open_database(const char *filename,sqlite3 **db)
{


	//open temperature_database
	int			rc;
	rc = sqlite3_open(filename,db);

	if(rc)
	{
		fprintf(stderr,"can not open database: %s\n",sqlite3_errmsg(*db));
		exit(0);
	}
	else
	{
		fprintf(stdout,"opened database successfully\n");
	}

	return 0;
}

//关闭数据库
int close_database(sqlite3 *db)
{
	sqlite3_close(db);	
}

//执行数据库
int execute_exec(sqlite3 *db,const char *sql)
{
	sqlite3_callback	callback;
	char				*zErrMsg = 0;
	int					rc;

	rc = sqlite3_exec(db,sql,callback,0,&zErrMsg);

	if(rc != SQLITE_OK)
	{
	
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	else
	{

		
		fprintf(stdout,"execute database successfully!\n");
	}
	return 0;
}



static int callback(void *NotUsed,int argc,char **argv,char **azColName)
{
	int 		i;
	for(i = 0;i < argc; i++)
	{
		printf("%s = %s\n",azColName[i],argv[i] ? argv[i] : "NULL");
	}
	printf("\n");

	return 0;
}
