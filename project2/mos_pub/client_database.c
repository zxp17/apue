/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client_database.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/05/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "09/05/22 19:08:38"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <sqlite3.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "client_database.h"
#include "logger.h"

static int callback(void *data,int argc,char **argv,char **azColName)
{
		int			i;
		fprintf(stderr,"%s:",(const char*)data);
		for(i = 0;i < argc; i++)
		{
			log_error("%s = %s\n",azColName[i],argv[i] ? argv[i] : "NULL");
		}
		log_info("\n");
		return 0;
}

int open_database(const char *filename,sqlite3 **db)
{
		char				*zErrMsg = 0;
		int					rc;
		char				*sql;

		rc = sqlite3_open(filename,db);

		if(rc)
		{
			fprintf(stderr,"can not open database: %s\n",sqlite3_errmsg(*db));
			return -1;
		}
		else
		{
			fprintf(stdout,"opened database successfully\n");
		}

		sql = 	"CREATE TABLE client_temper_database(" \
			 "SERIAL			char(20)					NOT NULL," \
				  "TIME			char(30)	PRIMARY KEY 	NOT NULL," \
			"TEMPERATURE		char(20)					NOT NULL);";

		rc = sqlite3_exec(*db,sql,0,0,&zErrMsg);

		if(rc != SQLITE_OK)
		{
			fprintf(stderr,"SQL error: %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(*db);
			return -2;
		}
		else
		{
			fprintf(stdout,"table created successfully\n");
		}
									
		return 0;
}
int save_database(sqlite3 *db,char *buf)
{
		char				*info[10] = {0};
		char				delims[2] = "\n";
		char				sql1[1024];
		int					rc;
		char				*result = NULL;
		int					i = 0;
		char				*zErrMsg = 0;

		result = strtok(buf,delims);

		while(result != NULL)
		{
			info[i] = result;
			log_debug("info[%d] is %s\n",i,info[i]);
			i++;
			result = strtok(NULL,delims);
		}
															
		snprintf(sql1,sizeof(sql1),"INSERT INTO %s(SERIAL,TIME,TEMPERATURE)VALUES('%s','%s','%s');","client_temper_database",info[0],info[1],info[2],info[3],info[4],info[5],info[6],info[7],info[8],info[9]);
															
		rc = sqlite3_exec(db,sql1,0,0,&zErrMsg);
															
		if(rc != SQLITE_OK)
		{
			fprintf(stderr,"SQL error: %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
		}
		else
		{
			fprintf(stdout,"insert successfully\n");
		}
		return 0;
}
int select_database(sqlite3 *db,char *s_data,int size)
{
	char			*sql;
	int				rc;
	char			*zErrMsg = 0;
	char			**result;
	int				row = 0;
	int				column = 0;
	const char		*data = "callback function called";
															
	sql = "SELECT * from client_temper_database limit 1";
															
	rc = sqlite3_get_table(db,sql,&result,&row,&column,&zErrMsg);
															
	if(rc != SQLITE_OK)
	{
		fprintf(stderr,"SQL errno: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return -1;
	}
	log_info("select from table successfully\n");
	
	if(0 == row)
	{
		sqlite3_free_table(result);
		return -1;
	}
	else
	{
		memset(s_data,0,size);
		snprintf(s_data,size,"%s\n%s\n",result[3],result[4],result[5]);
		sqlite3_free_table(result);
	}
	return 0;
}
int delete_database(sqlite3 *db)
{
	char			*sql;
	int				rc;
	char			*zErrMsg = 0;
														
	sql = "DELETE from client_temper_database where SERIAL in(select SERIAL from client_temper_database order by SERIAL limit 1);";
															
	rc = sqlite3_exec(db,sql,0,0,&zErrMsg);
														
	if(rc != SQLITE_OK)
	{
		fprintf(stderr,"SQL errno: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	else
	{
		fprintf(stdout,"delete successful\n");
	}
	return 0;
}
int close_database(sqlite3 *db)
{
	sqlite3_close(db);
}
