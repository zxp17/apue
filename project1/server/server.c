/*********************************************************************************
 *      Copyright:  (C) 2022 zhengxuping<1572077261@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  server.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(19/04/22)
 *         Author:  zhengxuping <1572077261@qq.com>
 *      ChangeLog:  1, Release initial version on "19/04/22 09:17:43"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <libgen.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <sqlite3.h>
#include "server.h"
//#include "gettime.h"
#include "create_database.h"

#define	MAX_EVENTS		512
//#define DEBUG


int main(int argc,char **argv)
{

	int					listenfd,connfd;
	int					serv_port = 0;
	int					daemon_run = 0;
	int					epollfd = 0;
	struct epoll_event	event;
	struct epoll_event	event_array[MAX_EVENTS];
	int					events;
	int					i;
	int					x = 0,y = 0;
	int					rv;
	char				buf[1024];
	int					opt;
	char				*progname = NULL;


	/* 
	 *the parsed data is stored in an array
	 * */
	char				*p = NULL;
	char				*p1 = (char*)malloc(1024);
	char				take_info[3][256] = {0};

	/* 
	 *sql knowledge
	 * */
	char				sql[2048];
	int					rc;
	sqlite3				*db;

	
	progname = basename(argv[0]);

	/* 
	 *command line parameter parsing
	 * */
	while((opt = getopt_long(argc,argv,"dp:h",long_options,NULL)) != -1)
	{
		switch(opt)
		{
			case 'd':
				daemon_run = 1;
				break;
			case 'p':
				serv_port = atoi(optarg);
				break;
			case 'h':
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

	set_socket_rlimit();


	if((listenfd = socket_server_init(NULL,serv_port)) < 0)
	{
		printf("BAD NEWS:  server listen on port %d failure\n",serv_port);
		return -2;
	}
	printf(" server start to listen on port %d\n",serv_port);


	if( daemon_run )		//set program running on background
	{
		daemon(0,0);
		//frist zero,change the working directory of the process to the following directory
		//second zero，redirects input and output errors to /dev/null
	}


	/* 
	 *specifies the maxmum values of the generated descriptoe
	 * */
	if((epollfd = epoll_create(MAX_EVENTS)) < 0)
	{
		printf("epoll_create() failure: %s\n",strerror(errno));
		return -3;
	}

	event.events = EPOLLIN;			
	event.data.fd = listenfd;		


	if( epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event) < 0 )
	{
		printf("epoll add listen socket failure: %s\n",strerror(errno));
		return -4;
	}


	for(;;)
	{

		//program will blocked here
		events = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);

		if(events < 0)
		{
			printf("epoll_wait failure: %s\n",strerror(errno));
			break;
		}
		else if(0 == events)		
		{
			printf("epoll_wait get timeout\n");
			continue;
		}

		for(i = 0; i < events; i++)
		{
			if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]: %s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
		
				close(event_array[i].data.fd);
			}
			
			if(event_array[i].data.fd == listenfd)
			{
				if((connfd = accept(listenfd,(struct sockaddr *)NULL,NULL)) < 0)
				{
					printf("accept new client failure: %s\n",strerror(errno));
					continue;
				}
#ifdef DEBUG
				printf("accept成功\n");
#endif
				event.data.fd = connfd;
				event.events = EPOLLIN;			
				

				/* 
				 *add the event to the list of interests
				 * */
				if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event) < 0)
				{
					printf("epoll add client socket failure: %s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
#ifdef DEBUG
				printf("epoll add new client socket[%d] ok\n",connfd);
#endif
			}

			else		//already connected client socket get data incoming
			{
				memset(buf,0,sizeof(buf));

				if((rv = read(event_array[i].data.fd,buf,sizeof(buf))) <= 0)
				{
					printf("socket[%d] read failure or get disconnect and will be removed\n",event_array[i].data.fd);
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{
					printf("\n\n");
					printf("socket[%d] read get %d bytes data from client and echo back: %s\n",event_array[i].data.fd,rv,buf);


					/* 
					 *get teh data to insert into the database
					 * */
					p = buf;
					x = 0;
					while((p1 = strchr(p,'\n')) != NULL)
					{
						
						strncpy(take_info[x],p,strlen(p) - strlen(p1));
						p = p1+1;
						x++;
					}
					strncpy(take_info[x],p,strlen(p));

					for(y = 0;y <= x; y++)
					{
#ifdef DEBUG
						printf("take_info[%d]: %s\n",y,take_info[y]);
#endif
					}
					free(p1);

					open_database("temperature_database.db",&db);

					snprintf(sql,sizeof(sql),"INSERT INTO %s(SERIAL,TIME,TEMPERATURE)VALUES('%s','%s','%s');","COMPANY",take_info[0],take_info[1],take_info[2]);

					execute_exec(db,sql);
					close_database(db);


					if(write(event_array[i].data.fd,buf,rv) < 0)
					{
						printf("sokcet[%d] write failure: %s\n",event_array[i].data.fd,strerror(errno));
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
					}
#ifdef DEBUG
					printf("write finish\n");
#endif
				}
			}
		}
	}
cleanup:
	close(listenfd);
	return 0;
}
/* 
 *initialzation of a socket
 * */
int socket_server_init(char *listen_ip,int listen_port)
{
	struct	sockaddr_in		serveraddr;
	int						rv = 0;
	int						on = 1;
	int						listenfd;
	
	/* 
	 *create a socket
	 * */
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("create socket failure: %d\n",strerror(errno));
		return -1;
	}
#ifdef	DEBUG
	printf("create socket sucessfully\n");
#endif

	//set socket port reuseable,fix 'address already in use' bug when socket server restart
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;			//set protocol family is AF_INET
	serveraddr.sin_port = htons(listen_port);	//converts the port number from host to network byte order

	/* 
	 *listen on all ports without specifying a port number
	 * */
	if( !listen_ip )	
	{
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		printf("i am listening all ports\n");
	}
	else	
	{

		/* 
		 *converts an IP address from dotted decimal notation to a binary integer
		 * */
		if(inet_pton(AF_INET,listen_ip,&serveraddr.sin_addr) <= 0)
		{
			printf("inet_pton() set listen ip failure\n");
			rv = -2;
			goto cleanup;
		}
	}

	/* 
	 *binds the address and port of the socket
	 * */
	if(bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		printf("bind tcp socket failure: %s\n",strerror(errno));
		rv = -3;
		goto cleanup;
	}

	/* 
	 *specifies the maximum number of socket queues in the kernel
	 * */
	if(listen(listenfd,77) < 0)
	{
		printf("listen failure: %s\n",strerror(errno));
		rv = -4;
		goto cleanup;
	}

cleanup:
	if( rv< 0 )
		close(listenfd);
	else
		rv = listenfd;

	return rv;

}

/*
 *command line parameters prompt for input
 * */
static inline void print_usage(char *progname)
{
	printf("usage: %s [OPTION]...\n",progname);

	printf("%s is a socket server program,which used to verify client and echo back string from it\n",progname);

	printf("\nmandatory arguents to long options and short options\n");
	printf("-d[daemon ] set program running on background\n");
	printf("-p[port ] socket server port address\n");
	printf("-h[help ] display some information\n");

	printf("\n example: %s -d -p 7777\n",progname);

	return ;
}

/* 
 *sets the maxmum number of file descriptors
 * */
void set_socket_rlimit(void)
{
	struct rlimit 	limit = {0};

	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);

#ifdef DEBUG
	printf("set socket open fd max count to %ld\n",limit.rlim_max);
#endif

}

