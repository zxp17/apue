/*********************************************************************************
 *      Copyright:  (C) 2022 Xiezongyi<1764417007@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  libgpiod-led.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(23/07/22)
 *         Author:  Xiezongyi <1764417007@qq.com>
 *      ChangeLog:  1, Release initial version on "23/07/22 09:47:17"
 *                 
 ********************************************************************************/

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#ifndef CONSUMER
#define	CONSUMER	"Consumer"
#endif

int controlLED(int led_flag)
{
	char				*chipname = "gpiochip0";
	unsigned int		line_num = 24;				
	unsigned int		val = led_flag;
	struct gpiod_chip	*chip;
	struct gpiod_line	*line;
	int 				i,ret;

	chip = gpiod_chip_open_by_name(chipname);
	if(!chip)
	{
		perror("open chip failed\n");
		goto end;
	}
	
	line = gpiod_chip_get_line(chip,line_num);
	if(!line)
	{
		perror("Get line failed\n");
		goto close_chip;
	}

	ret = gpiod_line_request_output(line,CONSUMER,0);
	if(ret < 0)
	{
		perror("Request line as output failed\n");
		goto release_line;
	}

	//Blink 20 times
	for(i = 7; i > 0; i--)
	{
		ret = gpiod_line_set_value(line,val);
		if(ret < 0)
		{
			perror("set line output failed\n");
			goto release_line;
		}
		printf("output %u on line #%u\n",val,line_num);
		sleep(1);
	}

release_line:
	gpiod_line_release(line);
close_chip:
	gpiod_chip_close(chip);
end:
	return 0;
}
