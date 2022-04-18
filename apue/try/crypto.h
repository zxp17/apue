/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  crypto.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(17/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/22 10:37:47"
 *                 
 ********************************************************************************/

#ifndef		_CRYPTO_H_
#define		_CRYPTO_H_


extern	int  encrypt(char *plaintext,char *ciphertext,int size);

extern	int  decrypt(char *ciphertext,char *plaintext,int size);

#endif
