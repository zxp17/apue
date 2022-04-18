#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>

int		g_sigstop = 0;

void signal_stop(int signum)
{
	if(SIGTERM == signum)		//SIGTERM kill命令默认发送的信号，默认动作是终止信号
	{
		printf("SIGTERM signal detected\n");
	}
	else if( SIGALRM == signum )	//SIGALRM alarm()系统调用发送的信号
	{
		printf("SIGALRM aignal detected\n");
		g_sigstop = 1;
	}
}
void signal_user(int signum)
{
	if(SIGUSR1 == signum)		//SIGUSR1 用户自定义的信号1
	{
		printf("SIGUSR1 signal detected\n");
	}
	else if(SIGUSR2 == signum)	//SIGUSR2用户自定义的信号2
	{
		printf("SIGSUR2 signal detected\n");
	}
	g_sigstop = 1;
}
void signal_code(int signum)
{
	if(SIGBUS == signum)		//SIGBUS 运行非本CPU相关编译器编译的程序
	{
		printf("SIGBUS signal detected\n");
	}
	else if(SIGILL == signum)	//SIGILL 非法的指令
	{
		printf("SIGILL signal detected\n");
	}
	else if(SIGSEGV == signum)	//SIGSEGV 段错误系统给程序发送的信号
	{
		printf("SIGSEGV signal detected\n");
	}
	exit(-1);
}
int main(int argc,char **argv)
{
	char			*ptr = NULL;
	struct sigaction	sigact, sigign;

	//Method1: use signal() install signal
	
	signal(SIGTERM,signal_stop);
	signal(SIGALRM,signal_stop);

	signal(SIGBUS,signal_code);
	signal(SIGILL,signal_code);
	signal(SIGSEGV,signal_code);

	//Method2:use sigaction() install signal
	
	//iniialize the catch signal structure
	
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigact.sa_handler = signal_user;

	//setup the ignore signal
	sigemptyset(&sigign.sa_mask);
	sigign.sa_flags = 0;
	sigign.sa_handler = SIG_IGN;

        sigaction(SIGINT,&sigign,0);	//ignore SIGINT signal by CTRL+C

	sigaction(SIGUSR1,&sigact,0);	// catch SIGUSR1
	sigaction(SIGUSR2, &sigact,0);	//catch  SIGUSR2

	printf("program start running for 20 seconds...\n");
	alarm(20);

	while( !g_sigstop )
	{
		;
	}
	printf("program start stop running..\n");

	printf("invalid pointer operator will raise SIGSEGV signal\n");

	*ptr = 'h';

	return 0;
}
