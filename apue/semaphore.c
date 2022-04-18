#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define FTOK_PATH	"/dev/zero"
#define FTOK_PROJID	0x22

union semun
{
	int			val;
	struct semid_ds 	*buf;		//信号量集合结构体
	unsigned short 		*array;
};

int semaphore_init(void);
int semaphore_p(int semid);
int semaphore_v(int semid);
void semaphore_term(int semid);

int main(int argc,char **argv)
{
	int		semid;
	pid_t		pid;
	int		i;

	if((semid = semaphore_init()) < 0)
	{
		printf("semaphore initial failure: %s\n",strerror(errno));
		return -1;
	}
	if((pid = fork()) < 0 )
	{
		printf("fork() failure: %s\n",strerror(errno));
		return -2;
	}
	else if(0 == pid)	//child process
	{
		printf("child process start running and do something now...\n");
		sleep(3);
		printf("child process do something over...\n");
		semaphore_v(semid);

		sleep(1);

		printf("child process exit now\n");
		exit(0);
	}
	//前面的semaphore_init()函数里将信号量的值设为0，如果这时候父进程先执行的话，p操作会阻塞 ，直到子进程执行v操作后，父进程的p操作才能返回继续执行
	printf("parent process p operator wait child process over\n");
	semaphore_p(semid);

	printf("parent process destroy semaphore and exit\n");
	sleep(2);
	printf("child process exit and ");

	semaphore_term(semid);
	return 0;
}
int semaphore_init(void)
{
	key_t		key;		//该值的声明类型是未来ftok()函数的返回值
	int		semid;
	union semun	sem_union;


	//ftok()函数说明，该函数根据第一个参数指定的文件或目录的索引节点号和第二个参数计算并返回一个key_t类型的ID值，如果失败返回-1；
	//第一个参数pathname是一个系统中必须存在的文件或文件夹的路径，会使用该文件的索引节点；
	//第二个参数FTOK_PROJID是用户指定的一个子序号买这个数字有点称之为project ID,他是一个8bits的整数，取值范围是1~255.
	if((key = ftok(FTOK_PATH,FTOK_PROJID)) < 0)
	{
	
		printf("ftok() get IPC token failure: %s\n",strerror(errno));
		return -1;
	}

	//semget()函数用来创建一个信号集，或者获取已存在的信号集，成功返回信号集的标识符，失败返回-1
	//参数说明：
	//第一个参数：key是所创建或打开信号量集的键值（ftok成果执行的返回值），不相关的进程可以通过它访问一个信号量，它代表程序可以要使用的某个资源。
	//第二个参数指定需要的信号量数目，它的终止几乎总是1；
	//第三个参数是一组标志，当想要当信号量不存在时创建一个新的信号量，可以和值IPC_CREAT做按位或操作。
	semid = semget(key,1,IPC_CREAT|0644);

	if(semid < 0)
	{
		printf("semget() get semid failure: %s\n",strerror(errno));
		return -2;
	}
	sem_union.val = 0;

	//semctl()函数，该函数用来初始化信号集，或者删除信号集，成功返回一个整数，失败返回一个-1
	//参数说明：
	//第一个参数时semget()函数返回的信号量键值
	//第二个参数是操作信号集中的编号，第一个信号是0；
	//第三个参数是在semid指定的信号集合上执行此命令，这里的SETVAL表示：设置信号量集中的一个单独的信号量的值，此时需要传入第四个参数
	//第四个参数是可选的，若使用该参数，其类型是semun，需要我们自己在代码中定义	
	if(semctl(semid,0,SETVAL,sem_union) < 0)
	{
		printf("semctl() set initial value failure: %s\n",strerror(errno));
		return -3;
	}
	printf("semaphore get key_t[0x%x] and semid[%d]\n",key,semid);

	return semid;
}
void semaphore_term(int semid)
{
	union semun	sem_union;

	//该函数用来初始化信号集，或者删除信号集，成功返回一个整数，失返回-1
	//这里是用来删除信号的
	if(semctl(semid,0,IPC_RMID,sem_union) < 0)
	{
		printf("semctl() delete semaphore ID failure: %s\n",strerror(errno));
	}
	return ;
	
}
int semaphore_p(int semid)
{
	struct sembuf	_sembuf;

	_sembuf.sem_num = 0;		//操作信号在信号集中的编号，第一个信号的编号是0，最后一个信号的编号是nsems-1
	_sembuf.sem_op = -1;		//操作 为负（P操作），其绝对值又大于信号的现有值，操作将会阻塞，直到信号值大于或等于sem_op的绝对值，通常用于获取资源的使用权
	_sembuf.sem_flg = SEM_UNDO;	//该值的初始化表示，程序结束时，保证信号值会被重设为semop()调用前的值。避免程序在异常情况下结束时未解锁锁定的资源，造成资源被永远锁定，造成死锁

	//semop()该函数用来操作一个信号或一组信号，也可以叫做PV操作，成功执行时，返回0，失败返回-1
	//参数说明：
	//第一个参数是semget()函数返回的信号量键值
	//第二个参数是一个指针，指向一个信号量操作数组
	//第三个参数是信号操作结构的数量，恒大于或等于1
	if(semop(semid,&_sembuf,1) < 0)
	{
		printf("semop p opertor failure: %s\n",strerror(errno));
		return -1;
	}
	return 0;
}
int semaphore_v(int semid)
{
	struct sembuf	_sembuf;

	_sembuf.sem_num = 0;
	_sembuf.sem_op = 1;
	_sembuf.sem_flg = SEM_UNDO;		//IPC_NOWAIT SEM_UNDO

	//该函数用来操作一个或一组信号，也可以叫做PV操作，成功执行时，返回0，失败返回-1
	if(semop(semid,&_sembuf,1) < 0)
	{
		printf("semop V opertor failure: %s\n",strerror(errno));
		return -1;
	}
	return 0;
}
