/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client_database.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(05/05/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "05/05/22 10:30:41"
 *                 
 ********************************************************************************/


int open_database(const char *filename,sqlite3 **db);

int save_database(sqlite3 *db,char *buf);

int select_database(sqlite3 *db,char *s_data,int size);

int delete_database(sqlite3 *db);

int close_database(sqlite3 *db);
