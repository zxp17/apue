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

static int callback(void *NotUsed,int argc,char **argv,char **azColName);
int open_database(const char *filename,sqlite3 **db);
int execute_exec(sqlite3 *db,const char *sql);
int close_database(sqlite3 *db);
