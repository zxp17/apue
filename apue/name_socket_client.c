#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define SOCKET_PATH	"/tmp/socket.domain1"
#define MSG_STR		"hello,unix domain sokcet server"

int main(int argc,char **argv)
{
	int			conn_fd = -1;
	int			rv = -1;
	char			buf[1024];
	struct sockaddr_un	serv_addr;

	conn_fd = socket(AF_UNIX,SOCK_STREAM,0);
	if(conn_fd < 0)
	{
		printf("create socket failure: %s\n",strerror(errno));
		return -1;
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path,SOCKET_PATH,sizeof(serv_addr.sun_path)-1);

	if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("connect to unix domain socket server on \"%s\" failure: %s\n",SOCKET_PATH,strerror(errno));
		return 0;
	}
	printf("connect to unix domain socket server on \"%s\" ok: %s\n",SOCKET_PATH);

	if(write(conn_fd,MSG_STR,strlen(MSG_STR)) < 0)
	{
		printf("write data to unix damain socket server on \"%s\" failure: %s\n",SOCKET_PATH,strerror(errno));
		goto Cleanup;
	}

	memset(buf,0,sizeof(buf));
	rv = read(conn_fd,buf,sizeof(buf));

	if(rv < 0)
	{
		printf("read data from server failure: %s\n",strerror(errno));
		goto Cleanup;
	}
	else if(0 == rv)
	{
		printf("client connect to server get disconnected\n");
		goto Cleanup;
	}
	printf("read %d bytes data from server: '%s'\n",rv,buf);

Cleanup:
	close(conn_fd);
}
