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
int open_database();
int execute_exec();
int close_database();

sqlite3		*db;
char		*zErrMsg = 0;
int			rc;
char		*sql;

  
int main(int argc,char *argv[])
{
	
//查看当前数据库中的数据
 /*   
	open_database();

	sql = "SELECT * from COMPANY";

	execute_exec();
	close_database();
*/
//删除数据库中指定的数据
/*  	

 	open_database();

	sql = "DELETE from COMPANY where SERIAL=26; " \
		   "SELECT * from COMPANY";

	execute_exec();

	close_database();
*/
//插入数据到数据库中
	
	open_database();

	sql = "INSERT INTO COMPANY (SERIAL,TIME,TEMPERATURE) " \
		   "VALUES ('222','212','222');";


	printf("sql: %s\n",sql);

	execute_exec();

	close_database();


	return 0;

}

//打开数据库
int open_database()
{

	//open temperature_database
	rc = sqlite3_open("temperature_database.db",&db);

	if(rc)
	{
		fprintf(stderr,"can not open database: %s\n",sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stdout,"opened database successfully\n");
	}

	return 0;
}

//关闭数据库
int close_database()
{
	sqlite3_close(db);	
}

//执行数据库
int execute_exec()
{
	rc = sqlite3_exec(db,sql,callback,NULL,&zErrMsg);

	printf("执行成功时rv: %d\n",rc);

	if(rc != SQLITE_OK)
	{
		printf("nonononononononon\n");
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("yesyesyesyesssssssss\n");
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
