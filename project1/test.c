/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(19/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "19/04/22 18:43:55"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc,char ** argv)
{
	char *cp = NULL;
	struct in_addr *inp = NULL;

	printf("请输入一段IP格式的地址：\n");
	scanf("%s",cp);

	inet_aton(cp,inp);

	printf("经过函数转换的ip地址为：%s",*inp);

	return 0;
}
