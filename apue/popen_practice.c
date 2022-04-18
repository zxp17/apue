#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int get_ipaddr(char *interface,char *ipaddr,int ipaddr_size);
//函数声明 通过参数：接口、ip地址、ip地址长度获取ip地址


int main(int argc,char **argv)
{
	char	ipaddr[16];
	char	*interface = "enp2s0";

	memset(ipaddr,0,sizeof(ipaddr));

	if(get_ipaddr(interface,ipaddr,sizeof(ipaddr)) < 0)
	{
		printf("ERROR: get IP address failure\n");
		return -1;
	}
	return 0;
}

int get_ipaddr(char *interface,char *ipaddr,int ipaddr_size)
{
	char	buf[1024];
	FILE	*fp;
	int	len;
	int	rv;

	if( !interface || !ipaddr || ipaddr_size < 16)	//当接口 或者 地址 或者IP地址的长度错误的时候，输出错误提示
	{
		printf("invalid input arguments\n");
		return -1;
	}

	memset(buf,0,sizeof(buf));
	snprintf(buf,sizeof(buf),"ifconfig %s",interface);


	if(NULL == (fp = popen(buf,"r")))
	{
		printf("popen() to excute command \"%s\" failure: %s\n",buf,strerror(errno));
		return -2;
	}

	rv = -3;
	//set default return value to -3 means parser failure


	while( fgets(buf,sizeof(buf),fp))
	{

		//fgets()函数其实就是一行一行的文件里面的内容
		printf("you can deal with you file in here\n");

		rv = 0;		//parser IP address ok and set rv to 0
		break;
	
	}
	return rv;
}
