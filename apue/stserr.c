#include <stdio.h>		//printf()和perror()在该头文件中声明
#include <errno.h>		//errno系统全局变量在该头文件中声明
#include <string.h>		//strerror()在该头文件中声明
#include <sys/types.h>		//open()系统调用需要的三个头文件
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	char *file_name = "/test.txt";		//根目录下并不存在该文件，在open()打开时会失败
	int   fd = -1;

	fd = open(file_name,O_RDONLY,006);
	if(fd < 0)
	{
		perror("open file failure");
		printf("open file %s failure: %s\n",file_name,strerror(errno));
		return 0;
	}
	close(fd);
}
