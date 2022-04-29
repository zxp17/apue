/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(28/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "28/04/22 17:35:07"
 *                 
 ********************************************************************************/
#include "create_database.h"

int main()
{
	char	*sql_test =NULL;


	open_database();

	sql_test = "INSERT INTO COMPANY (SERIAL,TIME,TEMPERATURE) " \
				"VALUES ('123','123','123');";

	execute_exec(sql_test);

	close_database();

	return 0;

}

