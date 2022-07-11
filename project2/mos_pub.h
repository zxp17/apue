/********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mos_pub.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(11/07/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "11/07/22 18:58:58"
 *                 
 ********************************************************************************/
struct trans_info
{
	char			sno[20];
	char			time[20];
	char			temperature[20];
};


int pack_info(struct trans_info *info,char *msg,int size);


