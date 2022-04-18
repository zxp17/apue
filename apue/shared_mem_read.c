#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define FTOK_PATH	"/dev/zero"
#define FTOK_PROJID	0x22

typedef struct st_student
{
	char		name[64];
	int		age;
}t_student;

int main(int argc,char **argv)
{
	key_t		key;
	int		shmid;
	int		i;
	t_student	*student;


	//ftok()该函数根据第一个参数指定的文件或目录的索引节点号和第二个参数计算并返回一个key_t类型的ID值，失败返回-1；
	if((key = ftok(FTOK_PATH,FTOK_PROJID)) < 0)
	{
		printf("ftok() get IPC token failure: %s\n",strerror(errno));
		return -1;
	}

	//shmget()该函数用来创建一个信号集，或者获取已存在的信号集，成功返回信号量的标识符，失败返回-1
	shmid = shmget(key,sizeof(t_student),IPC_CREAT|0666);
	if(shmid < 0)
	{
		printf("shmget() create shared memory failure: %s\n",strerror(errno));
		return -2;
	}

	//函数说明：第一次创建完共享内存时，它还不能被任何进程访问，shmat函数的作用就是用来启动对该共享内存的访问，并把共享内存连接到当前进程的地址空间，调用成功时返回一个指向共享内存第一个字节的指针，如果调用失败返回-1
	student = shmat(shmid,NULL,0);
	if((void *)-1 == student)
	{
		printf("shmat() alloc shared memory failure: %s\n",strerror(errno));
		return -2;
	}
	for(i = 0;i < 10; i++)
	{
		printf("student '%s' age [%d]\n",student->name,student->age);
		sleep(1);
	}
	//shmctl()该函数用于控制共享内存
	//参数说明：
	//第一个参数是shmget函数返回的共享内存标识符
	//第二个参数是要采取的操作，IPC_RMID表示删除共享内存段
	//第三个参数是一个结构指针，它指向共享内存模和访问权限的结构
	shmctl(shmid,IPC_RMID,NULL);

	return 0;
}
