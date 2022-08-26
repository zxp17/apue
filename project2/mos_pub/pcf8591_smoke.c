#include <stdio.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/ioctl.h> 
#include <linux/types.h> 
#include <sys/stat.h> 
#include <linux/i2c.h> 
#include <linux/i2c-dev.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <string.h> 
#include <stdint.h> 
#include <time.h> 
#include <errno.h> 
#include <string.h>

#include "logger.h"
#include "pcf8591_smoke.h"

#define I2C_API_RDWR 
#define ADC_CH1                         0x40	// 光敏电阻通道
#define ADC_CH2                         0x41	// 热敏电阻通道
#define ADC_CH3                         0x42	// 外接烟雾传感器通道
#define ADC_CH4                         0x43   	// 0到5v可调电压 

int getSmokescope(float *smoke) 
{        
        int fd;             

        fd = pcf8591_init();     
           
        if(fd < 0)        
        {                
                 log_error("pcf8591 initialize failure\n");                
                 return 1;        
        }
        
        if( pcf8591_get_adc(fd,smoke) < 0 )        
        {                
            	log_error("pcf8591 get get adc data\n");                
                return 2;        
        }
		log_info("smokescope: %f\n",*smoke);
        
        close(fd); 
 }
 
static inline void msleep(unsigned long ms) 
{        
        struct timespec 	cSleep;        
        unsigned long 		ulTmp;

        cSleep.tv_sec = ms / 1000;  
              
        if (cSleep.tv_sec == 0)        
        {                   
                ulTmp = ms * 10000;                
                cSleep.tv_nsec = ulTmp * 100;        
        }           
        else        
        {                   
                cSleep.tv_nsec = 0;        
        }   
        nanosleep(&cSleep, 0); 
}

int pcf8591_init(void) 
{        
        int     fd;

        if( (fd=open("/dev/i2c-1", O_RDWR)) < 0)        
        {                
                log_error("i2c device open failed: %s\n", strerror(errno));                
                return -1;        
        }
    
        ioctl(fd, I2C_TENBIT, 0);   
        ioctl(fd, I2C_SLAVE, 0x48); 

        return fd; 
}

int pcf8591_get_adc(int fd,float *smokescope) 
{        
        unsigned char buf[1];

        if( fd < 0)        
        {                
                printf("%s line [%d] %s() get invalid input arguments\n", __FILE__, __LINE__, __func__ );                   
                return -1;        
        }
     
        memset(buf, 0, sizeof(buf));        
        buf[0] = ADC_CH3;          
        write(fd, buf, 1);  
        
        // msleep(30);
        memset(buf, 0, sizeof(buf));        
        read(fd, buf, 1);  

		*smokescope = buf[0]/255.0*100;


        log_debug("data = %x\n", buf[0]);
        log_debug("smokescope = %.2f%\n", *smokescope); 

		return 0; 
}
