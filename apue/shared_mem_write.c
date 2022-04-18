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
	

	//ftok()该函数根据第一个参数指定的文件或目录的索引节点号和第二个参数计算并返回一个key_t类型的ID值，如果失败就返回-1；
	//
	if((key = ftok(FTOK_PATH,FTOK_PROJID)) < 0)
	{
		printf("ftok() get IPC token failure: %s\n",strerror(errno));
		return -1;
	}

	//shmget()该函数用来创建一个信号集，或者获取已存在的信号集，成功返回信号量集的标识符，失败返回-1
	shmid = shmget(key,sizeof(t_student),IPC_CREAT|0666);

	if(shmid < 0)
	{
		printf("shmget() create shared memory failure: %s\n",strerror(errno));
		return -2;
	}

	//shmat()函数说明：第一次创建完共享内存时，它还能被任何进程访问，shmat函数的作用就是用来启动对该共享内存的访问
	//并把共享内存连接到当前进程的地址空间，调用成功时返回一个指向共享内存第一个字节的指针，调用失败返回-1
	//参数说明：
	//第一个参数：shmid是由shmget函数返回的共享内存标识
	//第二个参数：指定共享内存连接到当前进程中的地址位置，通常为空，表示让系统来选择共享内存的地址
	//第三个参数是一组标志位，通常为0
	
	student = shmat(shmid,NULL,0);

	if((void *)-1 == student)
	{
		printf("shmat() alloc shared memory failure: %s\n",strerror(errno));
		return -2;
	}
	strncpy(student->name,"zhangsan",sizeof(student->name));
	student->age = 18;

	for(i = 0;i < 10; i++)
	{
		student->age++;
		printf("student '%s' age [%d]\n",student->name,student->age);
		sleep(1);
	}
	//shmdt()函数用于将共享内存从当前进程中分离，将共享内存中分离并不是删除它，指示使该共享内存对当前进程不再可用
	//student是shmat函数返回的地址指针，调用成功返回0，失败时返回-1；
	shmdt(student);

	//shmctl()该函数用于控制共享内存，IPC_RMID表示删除共享内存段
	shmctl(shmid,IPC_RMID,NULL);

	return 0;
}
