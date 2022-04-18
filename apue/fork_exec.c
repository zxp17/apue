#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define TMP_FILE	"/tmp/.ifconfig.log"
//标准输出重定向文件，/tmp路径是在Linux系统在内存里做的一个文件系统，放在这里不用写硬盘程序运行会快点

int main(int argc,char **argv)
{
	pid_t	pid;
	int 	fd;
	char	buf[1024];
	int 	rv;
	char	*ptr;
	FILE	*fp;
	char	*ip_start;
	char	*ip_end;
	char	ipaddr[16];

	//父进程打开这个文件，子进程将会继承父进程打开的这个文件描述符，这样父子进程都可以通过各自的文件描述符访问同一个文件了
	if( (fd = open(TMP_FILE,O_RDWR | O_CREAT | O_TRUNC,0644)) < 0 )
	{
		printf("Redirect standard output to file failure: %s\n",strerror(errno));
		return -1;
	}
	// 父进程开始创造进程
	pid = fork();
	if(pid < 0)
	{
		printf("fork() create child process failure: %s\n",strerror(errno));
		return -1;
	}
	else if( pid == 0 )
	{
		printf("child process start excute ifconfig program\n");

		dup2(fd,STDOUT_FILENO);
		//子进程会继承父进程打开的文件描述符，此时子进程重定向标准输出到父进程所打开的文件里
		execl("/sbin/ifconfig","ifconfig","enp2s0",NULL);

		//execl()函数并不会返回，因为他区执行另一个程序了，如果execl()返回了，说明系统调用出错了
		printf("child process excute another program,will not return here. return here means execl() error\n");
		return -1;
	}
	else
	{
		//父进程等待3秒，让子进程先执行
		sleep(3);
	}
	//子进程因为调用了execl()，他会丢掉父进程的文本段，所以子进程不会执行到这里了，只有父进程会继续执行这后面的代码
	memset(buf,0,sizeof(buf));

	//父进程这时候是读不到内容的，这时因为子进程往文件里写内容时已经将文件偏移量修改到文件尾了
	rv = read(fd,buf,sizeof(buf));
	printf("read %d bytes data directly read after child process write\n",rv);

	//父进程如果要读则需要将文件偏移量设置到文件头才能读到内容
	memset(buf,0,sizeof(buf));
	lseek(fd,0,SEEK_SET);
	rv = read(fd,buf,sizeof(buf));
	printf("read %d bytes data after lseek: \n %s",rv,buf);


	//如果使用read()读的话，一下子就读N多个字节进buffer，但有时我们希望一行一行地读取文件的内容，这时可以使用fdopen（）函数将文件描述符fd转成文件流fp
	
	fp = fdopen(fd,"r");

	fseek(fp,0,SEEK_SET);	//重新设置文件偏移量到文件头


	while( fgets(buf,sizeof(buf),fp) )	//fgets()从文件里一下子读一行，如果读到文件尾则返回NULL
	{
		//通过包含IP那一行没有二义性的关键字，在该行中找到该关键字就可以从这里面解析出IP地址
		if( strstr(buf,"netmask") )	
		{	
			//查找inet关键字，inet关键字后面跟的就是IP地址
			ptr = strstr(buf,"inet");	
			if( !ptr )
			{
				break;
			}
			ptr += strlen("inet");

			//inet关键字后面时空白符，我们不确定是空格还是tab，所以这里使用isblank()函数判断如果字符还是空白符就往后跳过；

			while( isblank(*ptr) )
				ptr++;

			//跳过空白符后跟着的就是IP地址的起始字符
			ip_start = ptr;

			//IP地址后面跟着的又是空白字符，跳过所有的非空白字符，即IP地址部分
			
			while( !isblank(*ptr) )
				ptr++;

			ip_end = ptr;

			//第一个空白字符的地址也就是IP地址终止的字符位置
			
			//使用memcpy()函数将IP地址拷贝到存放地址的buffer中，其中ip_end - ip_start就是IP地址的长度，ip_start就是IP地址的起始位置
			memset(ipaddr,0,sizeof(ipaddr));
			memcpy(ipaddr,ip_start,ip_end - ip_start);

			break;	
		}
	}
	printf("parser and get IP address: %s\n",ipaddr);

	fclose(fp);
	unlink(TMP_FILE);

	return 0;



}
