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
#include <sys/epoll.h>
#include <sys/resource.h>

#define MAX_EVENTS	512
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))

static inline void print_usage(char *progname);
int socket_server_init(char *listen_ip,int listen_port);
void set_socket_rlimit(void);

int main(int argc,char **argv)
{
	int			listenfd,connfd;
	int			serv_port = 0;
	int			daemon_run = 0;
	char			*progname = NULL;
	int			opt;
	int			rv;
	int			i,j;
	int			found;
	char			buf[1024];

	int			epollfd;
	struct epoll_event	event;
	struct epoll_event	event_array[MAX_EVENTS];
	int			events;

	struct option		long_options[] = 
	{
		{"daemon",no_argument,NULL,'b'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	progname = basename(argv[0]);

	//parser the command line parmeters
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
	if(!serv_port)
	{
		print_usage(progname);
		return -1;
	}
	set_socket_rlimit();		//set max open socket count

	if((listenfd = socket_server_init(NULL,serv_port)) < 0)
	{
		printf("ERROR: %s server listen on port &=%d failure\n",argv[0],serv_port);
		return -2;
	}
	printf("%s server start to listen on port %d\n",argv[0],serv_port);

	//set program running on backgroun
	if(daemon_run)
	{
		daemon(0,0);
	}

	//系统调用epoll_create()创建了一个新的epoll实例，其对应的兴趣列表初始化为空，参数size指定了我们想要
	//通过epoll实例来检查的文件描述符的个数，该参数并不是一个上限，而是告诉内核应该如何为内部数据结构划分初始大小
	//从linux2.6.8版以来，size参数被忽略不用
	if((epollfd = epoll_create(MAX_EVENTS)) < 0)
	{
		printf("epoll_create() failure: %s\n",strerror(errno));
		return -3;
	}

	//event.events = EPOLLIN|EPOLLEF;
	event.events = EPOLLIN;			//events字段是一个位掩码，它指定了我们为待检查的描述符fd上所感兴趣的事件集合
	event.data.fd = listenfd;		//data字段是一个联合体，当描述符fd稍后称为稍后称为就绪态时，联合的成员可用来指定传回给调用进程的信息


	//系统调用epoll_ctl()能够修改由文件描述符epfd所代表的epollfd实例中的兴趣列表，成功返回0，出错返回-1。
	//第一个参数是epoll_create()的返回值
	//第二个参数用来指定需要执行的操作，这里是EPOLL_CTL_ADD,表示将描述符listenfd添加到epoll实例中的兴趣列表中去
	//第三个参数知名了要修改兴趣列表中的哪一个文件描述符的设定
	//第四个参数是指向epoll_event的指针
	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event) < 0)
	{
		printf("epoll add listen socket failure: %s\n",strerror(errno));
		return -4;
	}
	for(;;)
	{
		//program will blocked here
		//系统调用epoll_wait()返回epoll实例中处于就绪态的文件描述符信息，单个epoll_wait()调用能够返回多个就绪态文件描述符的信息，
		//调用成功后该函数返回第二个参数event_array中的元素个数
		//
		//epoll_wait()参数说明
		//第一个参数是epoll_create()的返回值
		//第二个参数event_array所指向的结构体数组中返回的是有关就绪态文件描述符的信息，数组event_array的空间由调用者父子申请
		//第三个参数MAX_EVENTS指定event_array数组里包含的元素个数
		//第四个参数用来确定epoll_wait()的阻塞行为，-1表示，调用将一直阻塞，直到兴趣列表中的文件描述符上产生了哪个事件
		events = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
		if(events < 0)
		{
			printf("epoll failure:%s\n",strerror(errno));
			break;
		}
		else if(0 == events)
		{
			printf("epoll get timeout\n");
			continue;
		}

		//rv>0 is the active events count
		for(i = 0;i < events;i++)
		{
			//常量说明：EPOLLERR:有错误发生；EPOLLHUP:表示出现挂断
			if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]： %s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				//将文件描述符event[i].data.fd从epollfd的兴趣列表中移除
				close(event_array[i].data.fd);
			}
			//listen socket get event means new client tart connect now
			if(event_array[i].data.fd == listenfd)
			{
				if((connfd == accept(listenfd,(struct sockaddr *)NULL,NULL) ) < 0)
				{
					printf("accept new client failure: %s\n",strerror(errno));
					continue;
				}
				event.data.fd = connfd;

				//event.events = EPOLLIN|EPOLLEF;
				event.events = EPOLLIN;
				//EPOLLIN可读取非高优先级数据
				if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event) < 0)
				{
					printf("epoll add client socket failue: %s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new client socket[%d] ok\n",connfd);
			}
			else		//already connected client socket get data incoming
			{
				if((rv = read(event_array[i].data.fd,buf,sizeof(buf))) <= 0)
				{
					printf("socket[%d] read failue or get disconnect and will be remove\n",event_array[i].data.fd);
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{
					printf("socket[%d] read get %d bytes data\n",event_array[i].data.fd,rv);
					//convert letter from lowercase to uppercase
					for(j = 0;j < rv; j++)
					{
						buf[j] = toupper(buf[j]);
					}
					if(write(event_array[i].data.fd,buf,rv) < 0)
					{
						printf("socket[%d] write failue: %s\n",event_array[i].data.fd,strerror(errno));
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
					}
				}
			}
		}
	}
Cleanup:
	close(listenfd);
	return 0;

}
static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTION]...\n",progname);

	printf(" %s is a socket server program,which used to verify client and echo back string from it\n",progname);
	printf("\n Mandatory arguments to long options are mandatory for short options too:\n");

	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port ] socket server port address\n");
	printf(" -h[help ] display this help information\n");

	printf("\nexample: %s -b -p 7777\n",progname);
	return ;
}
int socket_server_init(char *listen_ip,int listen_port)
{
	struct sockaddr_in	servaddr;
	int			rv = 0;
	int			on = 1;
	int			listenfd;
	
	//socket()函数，一种根据指定的地址族、数据类型和协议来分配一个套接口的描述字及其所用资源的函数，简称创建一个套接口
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("use socket() to create a tcp socket failure: %s\n",strerror(errno));
		return -1;
	}


	//setsockopt()函数，用于任意类型、任意状态套接口的设置选项值，本函数仅定义了最高的“套接口”
	//set socket port reuseable, fix 'address already in use' bug when socket server restart
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(listen_port);

	if(!listen_ip)		//listen all the local IP address
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else			//listen the specified IP address
	{
		if(inet_pton(AF_INET,listen_ip,&servaddr.sin_addr) <= 0)
		{
			printf("inet_pton() set listen IP address failure\n");
			rv = -2;
			goto Cleanup;
		}
	}
	//bind()函数，将长度为addrlen的struct sockaddr类型的参数与listenfd绑定在一起，将sockfd绑定在某个端口上
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
	{
		printf("use bind() to bind the tcp sockt failure: %s\n",strerror(errno));
		rv = -3;
		goto Cleanup;
	}

	//listen()函数，用来“倾听”客户端的请求，第一个参数是服务端套接字，第二个参数是等待连接队列的最大长度
	//需要注意的是假定第二个参数是10，这个10并不表示客户端最大的连接数，实际上可以有很多很多的客户端。
	if(listen(listenfd,64) < 0)
	{
		printf("listen socket failure: %s\n",strerror(errno));
		rv = -4;
		goto Cleanup;
	}

Cleanup:
	if(rv < 0)
	{
		close(listenfd);
	}
	else
	{
		rv = listenfd;
	}
	return rv;
}
//set open file description count to max
void set_socket_rlimit(void)
{
	struct rlimit	limit = {0};
	
	//结构体中rlim_cur是要取得或设置的资源软限制的值，rlim_max是硬限制，这两个值之间存在一些约束
	getrlimit(RLIMIT_NOFILE,&limit );		//用来取得
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);		//用来设置

	//getrlimit()和setrlimit()函数的第一个参数需要一个要控制的资源，比如CPU、内存、文件描述符个数等等

	printf("set socket open fd max count to %d\n",limit.rlim_max);	
}
