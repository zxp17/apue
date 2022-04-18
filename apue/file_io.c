#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 1024
#define MSG_STR "hello xiaozheng\n"

int main(int argc,char *argv[])
{
	int		fd = -1;
	int		rv = -1;		//return value
	char 		buf[BUFSIZE];		//用来存放从文件中读到的数据

	fd=open("test.txt",O_RDWR|O_CREAT|O_TRUNC,0666);
	if(fd < 0)
	{
		perror("open/create filetest.txt failure");	//perror()函数可用打印系统调用出错的具体原因，它的参数只能跟一个字符串提示符
		return 0;
	}
	printf("open file returned file descriptor [%d]\n",fd);

	if((rv=write(fd,MSG_STR,strlen(MSG_STR))) < 0)
	{
		printf("write %d bytes into file failure:%s\n",rv,strerror(errno));
		goto cleanup;
	}

	memset(buf,0,sizeof(buf));
	lseek(fd,0,SEEK_SET);
	if((rv=read(fd,buf,sizeof(buf)))< 0)
	{
		printf("read data from file failure :%s\n",strerror(errno));
		goto cleanup;
	}
	printf("read %d bytes data from file:%s\n",rv,buf);

cleanup:
	close(fd);

	return 0;

}
