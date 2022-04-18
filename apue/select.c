#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define ARRAY_SIZE(x)		(sizeof(x)/sizeof(x[0]))

static inline void msleep(unsigned long ms);
static inline void print_usage(char *prognamme);
int socket_server_init(char *listen_ip, int listen_port);

int main(int argc,char **argv)
{

	char		*progname = NULL;
	int		opt;
	int		daemon_run = 0;
	int		serv_port = 0;
	int		i,j;
	int		fds_array[1024];
	int		listenfd,connfd;
	fd_set		rdset;
	int		rv;
	int		found;
	int		maxfd = 0;
	char		buf[1024];


	struct option		long_options[] = 
	{
		{"daemon",no_argument,NULL,'b'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	progname = basename(argv[0]);

	//parser the command line parameters
	while((opt = getopt_long(argc,argv,"bp:h",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'b':
				daemon_run = 1;
				break;
			case 'p':
				serv_port = atoi(optarg);
				break;
			case 'h':		//get help information
				print_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;
		}
	}
	if( !serv_port )	//没有在终端输入端口号，输出提示信息
	{
		print_usage(progname);
		return -1;
	}

	if((listenfd = socket_server_init(NULL,serv_port)) < 0)
	{
		printf("ERROR: %s server_init listen on port %d failure\n",argv[0],serv_port);
		return -2;
	}
	printf("%s server start to listen on port %d\n",argv[0],serv_port);

	//set program running on background
	if(daemon_run)
	{
		daemon(0,0);
	}

	for(i = 0; i < ARRAY_SIZE(fds_array); i++)
	{
		fds_array[i] = -1;
	}

	fds_array[0] = listenfd;

	for(;;)
	{
		FD_ZERO(&rdset);

		for(i = 0;i < ARRAY_SIZE(fds_array);i++)
		{
			if(fds_array[i] < 0)
			{
				continue;
			}
			maxfd = fds_array[i] > maxfd ? fds_array[i] : maxfd;
			FD_SET(fds_array[i],&rdset);
		}
		//program will blocked here
		rv = select(maxfd+1,&rdset,NULL,NULL,NULL);
		//select()函数允许程序监视多个文件描述符，等待所监视的一个或者多个文件描述符变为”准备好“的状态，即文件描述符不再是阻塞状态，可以用于某类IO操作了
		//第一个参数表示集合中所有文件描述符的范围，即所有文件描述符的最大值+1
		//第二个参数监视文件描述符的一个集合，看读取是不是不阻塞了
		//第三个参数监视文件描述符的一个集合，看写入是不是不阻塞了
		//第四个参数用来监视发生错误异常文件
		//第五个参数表示select返回之前的时间上限
		//在这里我们只在意读取的问题，所以其它三个参数都设置成了NULL

		if(rv < 0)
		{
			printf("select failure: %s\n",strerror(errno));
			break;
		}//返回值小于0的时候，输出提示信息

		else if(0 == rv)
		{
			//当返回值等于0的时候，表示超时了
			printf("select get timeout\n");
			continue;
		}

		//listen socke get event means new client start connect now
		if(FD_ISSET(listenfd,&rdset))		//该函数用于测试指定的文件描述符是否在该集合中，rdset对应于需要检测的可读文件描述符的集合
		{

			//accept()函数，返回值对应客户端套接字标识
			//第一个参数用来表示服务器端套接字
			//第二个参数用来保存客户端套接字对应的”地方“（包括客户端IP和端口号信息等）
			//第三个参数是第二个参数的占地大小
			if((connfd = accept(listenfd,(struct sockaddr *)NULL,NULL)) < 0 )
			{
				printf("accept new client failure: %s\n",strerror(errno));
				continue;
			}
			found = 0;
			for(i = 0;i<ARRAY_SIZE(fds_array);i++)
			{
				if(fds_array[i] < 0)
				{
					printf("accept new client[%d] and add it into array\n",connfd);
					fds_array[i] = connfd;
					found = 1;
					break;
				}
			}
			if(!found)
			{
				printf("accept new client[%d] but full,so refuse it\n",connfd);
				close(connfd);
			}
		}
		else	//data arrive from already connected client
		{
			for(i = 0;i < ARRAY_SIZE(fds_array);i++)
			{
				if(fds_array[i] < 0 || !FD_ISSET(fds_array[i],&rdset))
					continue;
				if((rv = read(fds_array[i],buf,sizeof(buf))) <= 0)

				{
					printf("socket[%d] read failure or get disconnect.\n",fds_array[i]);
					close(fds_array[i]);
					fds_array[i] = -1;
				}
				else
				{
					printf("socket[%d] read get %d bytes data\n",fds_array[i],rv);

					//covert letter from lowercase to uppercase
					for(j = 0;i < rv;j++)
						buf[j] = toupper(buf[j]);

						if(write(fds_array[i],buf,rv) < 0)
						{
							printf("socket[%d] write failure: %s\n",fds_array[i],strerror(errno));
							close(fds_array[i]);
							fds_array[i] = -1;
						}
				}
			}
		}
	}
Cleanup:
	close(listenfd);
	return 0;
}

static inline void msleep(unsigned long ms)
{
	struct timeval		tv;
	tv.tv_sec = ms/1000;
	tv.tv_usec = (ms%1000)*1000;

	select(0,NULL,NULL,NULL,&tv);
}
static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTION....\n]",progname);

	printf(" %s is a socket server program,which used to verify client and echo back string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short option too:\n");

	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port ] socket server port address\n");
	printf(" -h[help ] display this help information\n");

	printf("\n exampple: %s -b -p 8900\n",progname);
	return ;
}

//socket服务器端的初始化
int socket_server_init(char *listen_ip,int listen_port)
{
	int			listenfd;
	int			on = 1;
	int			rv = 0;
	struct sockaddr_in	servaddr;


	//使用socket()函数来创建套接字
	//第一参数地址族，也就是IP地址类型AF_INET表示的是IPv4的地址
	//第二个参数为数据传输方式/套接字类型，SOCK_STTREAM表示（流格式套接字/面向连接的套接字）
	//第三个参数表示传输协议，常用的有IPPROTO和IPPTOTO_UDP,分别表示TCP传输协议和UDP传输协议，
	//一般前两个参数就可以创建套接字了，操作系统会自动推演除协议类型
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("use socket() to create a TCP socket failure: %s\n",strerror(errno));
		return -1;
	}//若创建套接字失败，则输出提示信息

	//set socket port reusable, fix 'address already in use' bug when socket server restart
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//该函数用来设置参数listenfd所指定的socket状态，第二个参数欲设置的网络层，一般设置为SOL_SOCKET以存取socket层
	//第三个参数在这里表示允许在bind()过程中本地地址可重复使用
	//第四个参数代表欲设置的值
	//第五个参数为第四个参数的长度


	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(listen_port);

	if( !listen_ip)		//listen all the local ip address
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	}
	else		//listen the specified ip address
	{

		//inet_pton()函数功能：将IP地址从字符串格转换成网络地址的格式
		//函数原型如下：static int inet_pton(int af,const char *src,void *dst)
		//第一个参数是IPv4的协议
		//第二个参数src是个指针，指向保存ip地址字符串形式的字符串
		//第三个参数dst指向存放网络地址的结构体的首地址
		if(inet_pton(AF_INET,listen_ip,&servaddr.sin_addr) <= 0)
		{
			printf("inet_pton() set listen ip address failure\n");
			rv = -2;
			goto Cleanup;
		}//如果转换出错就输出提醒信息
	}

	//在建立套接字文件描述符成功之后，需要对套接字进行地址和端口的绑定，才能进行数据的接收的发送操作
	//第一个参数是创建socket()函的时候的文件描述符
	//第二个参数是是指向结构为sockaddr参数的指针，sockaddr中包含了地址、端口和IP地址的信息需要注意的是，需要将结构体中的信息先设定之后再进行绑定
	//第三个参数是第二个参数结构的长度
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
	{
		printf("use bind() to bind the tcp socket failure: %s\n",strerror(errno));
		rv = -3;
		goto Cleanup;
	}//若绑定失败则输出提示信息


	//通过listen()函数，可以让套接字进入被动监听状态，listenfd为需要进入监听状态的套接字，
	//第二个参数为请求队列的最大长度
	//所谓被动监听，是指当没有客户端请求的时候，套接字处于“睡眠”状态，只有当接收到客户端请求是，套接字才会被“唤醒”来响应请求
	if(listen(listenfd,42) < 0)
	{
		printf("use bind() to bind the tcp socket failure: %s\n",strerror(errno));
		rv = -4;
		goto Cleanup;
	}//”被动“失败，输出提示消息
Cleanup:
	if(rv<0)
	{
		close(listenfd);
	}
	else
	{
		rv = listenfd;
	}
	return rv;



}
