/********************************************************************************
 *      Copyright:  (C) 2022 Xiezongyi<1764417007@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  pcf8591_smoke.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(22/08/22)
 *         Author:  Xiezongyi <1764417007@qq.com>
 *      ChangeLog:  1, Release initial version on "22/08/22 17:23:06"
 *                 
 ********************************************************************************/


int getSmokescope(float *smoke);

static inline void msleep(unsigned long ms);

int pcf8591_init(void);

int pcf8591_get_adc(int fd,float *smokescope);

