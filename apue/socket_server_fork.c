#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

void print_usage(char *program)
{
	printf("%s usage: \n",program);
	printf("-p(--port): sepcify server listen port.\n");
	printf("-h(--Help): print this help information.\n");
	return ;
}
int main(int argc,char **argv)
{
	int			sockfd = -1;
	int			rv = -1;
	struct sockaddr_in	servaddr;
	struct sockaddr_in	cliaddr;
	socklen_t		len;
	int			port = 0;
	int			clifd;
	int			ch;
	int			on = 1;
	pid_t			pid;

	struct option	opts[] = {
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while(( ch = getopt_long(argc,argv,"p:h",opts,NULL))  != -1)
	{
		switch(ch)
		{
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}
	

	if( !port )
	{
		print_usage(argv[0]);

		printf("port is what: %d\n",port);
		printf("process is excuting now!!!!!!!!!!!!1");

		return 0;
	}

	sockfd = socket(AF_INET,SOCK_STREAM,0);

	if(sockfd < 0)
	{
		printf("create socket failure: %s\n",strerror(errno));
		return -1;
	}
	printf("create socket[%d] successfully!\n",sockfd);

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&servaddr,0,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//listen all the IP address on this host
	//inet_aton("192.168.0.16",&servaddr.sin_addr);
	//only listen specify IP address on this host
	
	rv = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if( rv < 0 )
	{
		printf("Socket[%d] bind on port[%d] failureL %s\n",sockfd,port,strerror(errno));
		return -2;
	}

	listen(sockfd,13);
	printf("start to listen on port[%d]\n",port);

	while(1)
	{
		printf("start accept new client incoming!!!\n");

		clifd = accept(sockfd,(struct sockaddr *)&cliaddr,&len);
		if(clifd < 0)
		{
			printf("accept new client failure: %s\n",strerror(errno));
			continue;
		}
		printf("accept new client{%s:%d} succeddfully\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

		pid = fork();
		if(pid < 0)
		{
			printf("fork() create child process failure: %s\n",strerror(errno));
			close(clifd);
			continue;
		}
		else if( pid > 0 )
		{
			//parent process close client fd goes to accept new socket client again
			close(clifd);
			continue;
		}
		else if( 0 == pid )
		{
			char	buf[1024];
			int	i;

			printf("child process start to commuicate with socket client!!!\n");
			close(sockfd);
			//child process close the listen socket fd
			

			while(1)
			{
			
				memset(buf,0,sizeof(buf));
				rv = read(clifd,buf,sizeof(buf));

				if(rv < 0)
				{
					printf("read data from sockfd[%d] failure: %s\n",clifd,strerror(errno));
					close(clifd);
					exit(0);
				}
				else if(rv == 0)
				{
					printf("socket[%d] get disconnet\n",clifd);
					close(clifd);
					exit(0);
				}
				else if(rv > 0)
				{
					printf("read %d bytes data from server: %s\n",rv,buf);
				}

				//convert letter from lowercase to uppercase
				for(i = 0; i < rv; i++)
				{
					buf[i] = toupper(buf[i]);

				}
				rv = write(clifd,buf,rv);
				if(rv < 0)
				{
					printf("write to client by sockfd[%d] failure: %s\n",clifd,strerror(errno));
					exit(0);
				}

			}//child process loopp
		}//child process strat
	}
	close(sockfd);

	return 0;
}
