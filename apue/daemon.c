#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <libgen.h>		//basename

int main(int argc,char ** argv)
{
	char		*progname = basename(argv[0]);

	if( daemon(0,0) < 0 )		//创建守护进程失败的话，输出提示信息
	{
		printf("program daemon() failure: %s\n",strerror(errno));
		return -1;
	}
	openlog("daemon",LOG_CONS | LOG_PID,0);		
	//第一个参数daemon，是一个标记，加在每行日志的前面标识这个日志
	//第二个参数LOG_CONS,表示“如果将信息发送给syslogd守护进程时发生错误，直接将相关信息输出到终端”;LOG_PID表示每条日志信息中都包含进程号
	//第三个参数：指定记录消息程序的类型

	syslog(LOG_NOTICE,"program '%s' start running\n",progname);
	//syslog()函数与文件系统调用printf使用方法类似，第一个参数表示消息的级别
	//LOG_NOTICE表示不是错误情况，但是可能需要处理

	syslog(LOG_WARNING,"program '%s' running with a warning message\n",progname);
	//LOG_WARNING表示警告信息


	syslog(LOG_EMERG,"program '%s' running with a emergency message\n",progname);
	//LOG_EMERG表示紧急情况

	while(1)
	{
		
		;
	}
	syslog(LOG_NOTICE,"program '%s' stop running\n",progname);
	closelog();

	return 0;
}
