/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  create_table.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "21/04/22 14:37:35"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

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

int main(int argc,char *argv[])
{
	sqlite3			*db;
	char			*zErrMsg = 0;
	int				rc;
	char			*sql;

	//open temperature_database
	rc = sqlite3_open("temperature_database.db",&db);
	
	if(rc)
	{
		fprintf(stderr,"can not open database: %s\n",sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stdout,"opened temperature_database successfully\n");
	}

	//create SQL statement
	
	sql = "CREATE TABLE COMPANY(" \
		   "Equipment_serial_number		char(50)				NOT NULL, " \
		   "time	 			 		char(50) PRIMARY KEY	NOT NULL, " \
		   "temperature                 char(50)    			NOT NULL);";

	//execute SQL statement
	
	rc = sqlite3_exec(db,sql,callback,0,&zErrMsg);

	if(rc != SQLITE_OK)
	{
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout,"table create successfully\n");
	}
	sqlite3_close(db);

	return 0;
}
