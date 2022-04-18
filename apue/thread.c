#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread_worker1(void *args);
void *thread_worker2(void *args);

int main(int argc,char **argv)
{
	int		shared_var = 1000;
	pthread_t	tid;
	pthread_attr_t	thread_attr;

	if( pthread_attr_init(&thread_attr) )		//将该线程初始化
	{
		printf("pthraed_attr_init() failure: %s\n",strerror(errno));
		return -1;
	}
	if( pthread_attr_setstacksize(&thread_attr,120*1024) )	//设置线程栈的大小
	{
		printf("pthread_attr_setstacksize() failure: %s\n",strerror(errno));
		return -1;
	}
	if( pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED) )	//设置线程的属性为分离状态
	{
		printf("pthread_sttr_setdetachstate() failure: %s\n",strerror(errno));
		return -1;
	}

	printf("sucessfully excute!!!");

	pthread_create(&tid,&thread_attr,thread_worker1,&shared_var);
	//创建了一个线程，该线程的执行任务是去执行函数thread_worker1,shared_var是将要去执行的这个函数的参数
	//第二个参数中的thread_attr设置了属性PTHREAD_CREATE_DEtACHED，表示将线程设置成了分离状态

	printf("thread worker1 tid[%ld] created ok\n",tid);

	pthread_create(&tid,NULL,thread_worker2,&shared_var);
	//创建了一个线程，该线程的任务是去执行函数thread_worker2,balbala同上
	//但是没有设置线程的属性

	printf("thread worker2 tid[%ld] created ok\n",tid);

	pthread_attr_destroy(&thread_attr);	//在线程属性使用完之后，我们应该调用该函数把它摧毁释放


	//wait until thread worker2 exit()
	
	pthread_join(tid,NULL);		//以阻塞的方式等待thread指定的线程结束
					//当函数返回时，被等待的线程的资源被收回
					//如果线程结束，该函数会立即返回

	while(1)
	{
		printf("main/control thread shared_var: %d\n",shared_var);
		sleep(10);
	}

	return 0;
}
void *thread_worker1(void *args)
{
	int		*ptr = (int *)args;

	if(!args)
	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}
	printf("thread worker 1 {%ld} start running...\n",pthread_self());

	while(1)
	{
		printf("+++: %s before shared_var++: %d\n",__FUNCTION__,*ptr);
		*ptr += 1;
	       	sleep(2);
		printf("+++: %s after sleep shared_var: %d\n",__FUNCTION__,*ptr);
	}
	printf("thread worker 1 exit...\n");

	return NULL;
}
void *thread_worker2(void *args)
{
	int		*ptr = (int *)args;

	if( !args )
	{
		printf("%s() get invalid arguments\n",__FUNCTION__);
		pthread_exit(NULL);
	}

	printf("thread worker 2 [%ld] start running...\n",pthread_self());

	while(1)
	{
		printf("---: %s before shared_var++: %d\n",__FUNCTION__,*ptr);
		*ptr += 1;
		sleep(2);
		printf("---:%s after sleep ahsred_var: %d\n",__FUNCTION__,*ptr);
	}

	printf("thread worker 2 exit...\n");

	return NULL;
}
