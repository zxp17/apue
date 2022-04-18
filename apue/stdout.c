#include <stdio.h>			//printf在该头文件中声明
#include <unistd.h>			//STDOUT_FILENO，stdout等在该头文件中声明
#include <string.h>			//strlen在该头文件中声明，具体可使用man strlen查看
#define MSG_STR "hello\n"

int main(int main,char *argv[])
{
	printf("%s",MSG_STR);
	fputs(MSG_STR,stdout);
	write(STDOUT_FILENO,MSG_STR,strlen(MSG_STR));

	return 0;
}
