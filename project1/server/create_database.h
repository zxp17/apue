/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  create_database.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(08/05/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "08/05/22 20:32:09"
 *                 
 ********************************************************************************/
#ifndef		_CREATE_DATABASE_H_
#define		_CREATE_DATABASE_H_

/* 
 * The callback function
 * */
static int callback(void *NotUsed,int argc,char **argv,char **azColName);

/* 
 *This method opens the database 
 * */
int open_database(const char *filename,sqlite3 **db);

/* 
 *execute sql statement
 * */
int execute_exec(sqlite3 *db,const char *sql);

/* 
 *close the database
 * */
int close_database(sqlite3 *db);

#endif
