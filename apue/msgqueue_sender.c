#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FTOK_PATH	"/dev/zero"
#define FTOK_PROJID	0x22

typedef struct s_msgbuf
{
	long	mtype;
	char	mtext[512];
}t_msgbuf;

int main(int argc,char **argv)
{
	key_t		key;
	int		msgid;
	t_msgbuf	msgbuf;
	int		msgtype;
	int		i;


	//ftok()函数根据第一个参数指定的文件或目录的索引节点号和第二个参数计算并返回一个key_t类型的ID值，如果失败则返回-1
	if((key = ftok(FTOK_PATH,FTOK_PROJID)) < 0)
	{
		printf("ftok() get IPC token failure: %s\n",strerror(errno));
		return -1;
	}

	//msgget()该函数用来创建消息队列ID，成功返回一个非负整数，即共享内存段内的标识码，失败返回--1	
	msgid = msgget(key,IPC_CREAT|0666);

	if(msgid < 0)
	{
		printf("shmget() create shared memory failure: %s\n",strerror(errno));
		return -2;
	}
	msgtype = (int)key;

	printf("key[%d] msgid[%d] msgtype[%d]\n",(int)key,msgid,msgtype);

	for(i = 0;i < 10; i++)
	{
		msgbuf.mtype = msgtype;
		strcpy(msgbuf.mtext,"ping");


		//msgsnd()该函数用来发送一个消息，必须要有写消息队列的权限，成功返回0，失败返回-1
		if(msgsnd(msgid,&msgbuf,sizeof(msgbuf.mtext),IPC_NOWAIT) < 0)
		{
			printf("msgsnd() send message failure: %s\n",strerror(errno));
			break;
		}
		printf("send message: %s\n",msgbuf.mtext);
		sleep(1);
	}
	//msgctl()该函数用于控制消息队列，参数IPC_RMID表示删除消息队列
	msgctl(msgid,IPC_RMID,NULL);

	return 0;
}

