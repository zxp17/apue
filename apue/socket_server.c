#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTEN_PORT 8889
#define BACKLOG 	13

int main()
{
	int rv = -1;
	int listen_fd, 		 client_fd = -1;
	struct sockaddr_in	 serv_addr;
	struct sockaddr_in	 cli_addr;
	socklen_t		 cliaddr_len;
	char			 buf[1024];

	listen_fd = socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd < 0)
	{
		printf("create socket failure: %s\n",strerror(errno));
		return -1;
	}
	printf("socket create fd[%d]\n",listen_fd);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(LISTEN_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listen_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("create socket failure: %s\n",strerror(errno));
		return -2;
	}
	printf("socket[%d] bind on port[%d] for all IP address ok\n",listen_fd,LISTEN_PORT);

	listen(listen_fd,BACKLOG);

	while(1)
	{
		printf("\n start waiting and accept new client connect...\n");
		client_fd = accept(listen_fd,(struct sockaddr*)&cli_addr,&cliaddr_len);

		if(client_fd < 0)
		{
			printf("accept new socket failure:%s\n",strerror(errno));
			return -2;
		}
		printf("accept new client[%s:%d] with fd [%d]\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port),client_fd);
	

		memset(buf,0,sizeof(buf));
		if((rv = read(client_fd,buf,sizeof(buf))) < 0)
		{
			printf("read data from client socket[%d] failure: %s\n",client_fd,strerror(errno));
			close(client_fd);
			continue;
		}
		else if(rv == 0)
		{
			printf("client socket[%d] disconnnect\n",client_fd);
			close(client_fd);
			continue;
		}
		printf("read %d bytes data from client[%d] and echo it back: '%s'\n",rv,client_fd,buf);

		if(write(client_fd,buf,rv) < 0 )
		{
			printf("write %d bytes data back to client[%d] failure: %s\n",rv,client_fd,strerror(errno));
			close(client_fd);

		}
		sleep(1);
		close(client_fd);
	}
	close(client_fd);
}


