#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define FTOK_PATH	"/dev/zero"
#define FTOK_PROJID	0x22

typedef struct m_msgbuf
{
	long		mtype;
	char		mtext[512];
}t_msgbuf;

int main(int argc,char ** argv)
{
	key_t		key;
	int		msgid;
	t_msgbuf	msgbuf;
	int		msgtype;
	int		i;

	//ftok()该函数根据第一个参数指定的文件或目录的索引节点号和第二个参数计算并返回一个key_t类型的ID值
	if((key = ftok(FTOK_PATH,FTOK_PROJID)) < 0)
	{
		printf("ftok() get IPC token failure: %s\n",strerror(errno));
		return -1;
	}
	//msgget()该函数用来发送一个消息，必须要有写消息队列的权限，成功返回0，失败返回-1
	msgid = msgget(key,IPC_CREAT|0666);

	if(msgid < 0)
	{
		printf("shmget() create shared memeory failure: %s\n",strerror(errno));
		return -2;
	}
	msgtype = (int)key;
	printf("key[%d] msgid[%d] msgtype[%d]\n",(int)key,msgid,msgtype);

	for(i = 0; i <  10; i++)
	{
		memset(&msgbuf,0,sizeof(msgbuf));


		//msgrcv()该函数用来从一个消息队列接收信息，成功返回实际放到接收缓冲区里去的字符个数，失败返回-1
		//第一个参数时msgget函数返回的消息队列的ID
		//第二个参数是一个指针，它指向准备接收的信息
		//第三个参数第二个参数指向信息的长度
		//第四个参数是消息的类型
		//第五个参数控制着队列中没有相应类型的消息可供接收时将要发生的事
		if(msgrcv(msgid,&msgbuf,sizeof(msgbuf.mtext),msgtype,IPC_NOWAIT) < 0)
		{
			printf("msgsnd() receive message failure: %s\n",strerror(errno));
			break;
		}
		printf("receive message: %s\n",msgbuf.mtext);
		sleep(1);
	}
	//msgctl()该函数用于控制消息队列，参数IPC_RMID表示删除消息队列
	msgctl(msgid,IPC_RMID,NULL);

	return 0;
}
