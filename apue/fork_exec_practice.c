#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

//标准输出重定向的文件
#define TMP_FILE1	"/tmp/.ifconfig.log"

int main(int argc,char ** argv)
{
	
	//变量的定义
	int 		fd;
	pid_t 		pid;
	int 		rv;
	char		buf[1024];
	//父进程打开这个进程，子进程会继承父进程的文件描述符，所以父子进程可以通过各自的文件描述符来访问同一个文件了
	if((fd = open(TMP_FILE1,O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)
	{
		printf("redirect standard output to file failure:%s\n",strerror(errno));
		return -1;
		//打开（创建）文件出错就输出提示信息
	}
	//父进程开始创建子进程
	pid = fork();
	if(pid < 0)
	{
		//pid小于的时候，fork()系统调用出错，
		printf("fork() create child process failure:%s\n",strerror(errno));
		return -1;
	}
	else if(pid == 0)
	{
		//pid等于0的时候，子进程开始运行
		printf("child process start excute ifconfig program");

		//此时子进程重定向标准输出到父进程所打开的文件里面
		dup2(fd,STDOUT_FILENO);

		execl("/sbin/ifconfig","ifconfig","enp2s0",NULL);

		//子进程在上一条代码之后会去执行另一个程序，后面的代码不会再去执行，如果执行了，表示出错了
		printf("child process excute another failure:%s\n",strerror(errno));
		return -1;
	}
	else
	{
		//当pid大于0的时候，父进程执行
		sleep(3);
		
		//子进程将另一个程序的输出到文件中去之后，文件的偏移量（光标）这会读不出文件的内容
		lseek(fd,0,SEEK_SET);
		//将文件偏移量设置在最开始的地方

		rv = read(fd,buf,sizeof(buf));
		printf("read %d bytes data after lseek:%s \n",rv,buf);

	}
	close(fd);
	unlink(TMP_FILE1);
	return 0;
}
