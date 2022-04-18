#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	int fd = -1;
	fd = open("std.txt",O_RDWR|O_CREAT|O_TRUNC,0666);
	if(fd < 0)
	{
		printf("openc file failure:%s\n",strerror(errno));
		return 0 ;
	}	


	dup2(fd,STDIN_FILENO);		//标准输出重定向到std.txt文件中去
	dup2(fd,STDOUT_FILENO);		//标准输出重定向到std.txt文件中去
	dup2(fd,STDERR_FILENO);		//标准出错重定向到std.txt文件中去

	printf("fd = %d\n",fd);

	close(fd);
}
