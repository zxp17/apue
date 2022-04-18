#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int g_var = 6;
char g_buf[] = "a string write to stdout\n";

int main(int argc,char **argv)
{
	int var = 88;
	pid_t   pid;

	if( write(STDOUT_FILENO,g_buf,sizeof(g_buf) -1) < 0)
	{
		printf("\n\n");
		printf("write string to stdout error: %s\n",strerror(errno));
		return -1;
	}

	printf("before fork\n");

	if( (pid = fork()) < 0 )
	{
		printf("\n");
		printf("fork() error: %s\n",strerror(errno));
		return -2;
	}
	else if(0 == pid)
	{
		printf("\n");
		printf("child process PID[%d] running!!!\n",getpid());
		g_var++;
		var++;
	}
	else
	{
		printf("\n");
		printf("parent process PID[%d] waiting!!!\n",getpid());
		sleep(1);
	}

	printf("PID = %ld,g_var = %d,var = %d\n",(long)getpid(),g_var,var);
	return 0;
}
