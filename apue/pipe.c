#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MSG_STR "this message is from parent: hello child process"

int main(int argc,char ** argv)
{
	int		pipe_fd[2];
	int		rv;
	int		pid;
	char		buf[512];
	int		wstatus;

	if(pipe(pipe_fd) < 0)
	{
		printf("create pipe failure: %s\n",strerror(errno));
		return -1;
	}

	if((pid = fork()) < 0)
	{
		printf("create child process failure: %s\n",strerror(errno));
		return -2;
	}
	else if(0 == pid)
	{
		//child process close write endpoint,then read data from parent process
		close(pipe_fd[1]);

		memset(buf,0,sizeof(buf));
		rv = read(pipe_fd[0],buf,sizeof(buf));

		if(rv < 0)
		{
			printf("child process read from pipe failure: %s\n",strerror(errno));
			return -3;
		}
		printf("child process read %d bytes data from pipe: \"%s\"\n",rv,buf);
		return 0;
	}
	//parent process close read endpoint,then write data to child process
	if(write(pipe_fd[1],MSG_STR,strlen(MSG_STR)) < 0)
	{
		printf("parent process write data to pipe failue: %s\n",strerror(errno));
		return -3;
	}
	printf("parent start wait child process exit...\n");
	wait(&wstatus);
	//wait()函数主要是用来回收子进程资源，回收同时还可以得知被回收子进程的pid和退出状态
	//wstatus用来接收子进程返回的状态值

	return 0;

}
