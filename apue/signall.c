#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int g_child_stop = 0;
int g_parent_run = 0;

void sig_child(int signum)
{
	if( SIGUSR1 == signum )
	{
		g_child_stop = 1;
	}
}
void sig_parent(int signum)
{
	if(SIGUSR2 == signum)
	{
		g_parent_run = 1;
	}
}
int main(int argc,char ** argv)
{
	int		pid;
	int 		wstatus;

	signal(SIGUSR1,sig_child);
	signal(SIGUSR2,sig_parent);

	if((pid = fork()) < 0)
	{
		printf("create child process failure: %s\n",strerror(errno));
		return -2;
	}
	else if(pid == 0)
	{
		//child process can do something first here,then tell parent process to start running
		printf("child process start running and send parent a signal\n");

		kill(getppid(),SIGUSR2);

		while(!g_child_stop)
		{
			sleep(1);
		}
		printf("child process receive signal from parent and exit now\n");
		return 0;
	}
	printf("parent hangs up until receive signal from child\n");

	while(!g_parent_run)
	{
		sleep(1);
	}

	//parent process can do something here,then tell child process to exit
	
	printf("parent start running now and send child a signal to exit\n");

	kill(pid,SIGUSR1);

	//parent wait child process exit
	wait(&wstatus);
	printf("parent wait child process die and exit now\n");

	return 0;
}
