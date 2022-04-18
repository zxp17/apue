/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  crypto.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/22 10:31:35"
 *                 
 ********************************************************************************/

#include <string.h>

int encrypt(char *plaintext,char *ciphertext,int size)
{
	int		i;

	if(size < strlen(plaintext))
	{
		return -1;
	}
	for(i = 0; i < strlen(plaintext); i++)
	{
		ciphertext[i] = plaintext[i] + 3;
	}
	return 0;
}
int decrypt(char *ciphertext,char *plaintext,int size)
{
	int		i;

	if(size < strlen(ciphertext))
	{
		return -1;
	}
	for(i = 0;i < strlen(ciphertext); i++)
	{
		plaintext[i] = ciphertext[i] - 3;
	}

	return 0;
}
