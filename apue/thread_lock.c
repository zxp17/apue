#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread_worker1(void *args);
void *thread_worker2(void *args);

typedef struct worker_zxp_s		//给执行函数传递参数的时候，除了传递共享的变量以外，还有一个互斥锁，所以定义了一个结构体
{
	int		shared_var;
	pthread_mutex_t	lock;
}worker_zxp_t;

int main(int argc,char **argv)
{
	worker_zxp_t		worker_zxp;	//定义了传给子进程的变量参数
	pthread_t		tid;
	pthread_attr_t		thread_attr;

	worker_zxp.shared_var = 1000;
	pthread_mutex_init(&worker_zxp.lock,NULL);	//该函数用来初始化互斥锁

	if(pthread_attr_init(&thread_attr))
	{
		printf("pthread_attr_init() failure: %s\n",strerror(errno));
		return -1;
	}
	if(pthread_attr_setstacksize(&thread_attr,120*1024))
	{
		printf("pthread_attr_setstacksize() failure: %s\n",strerror(errno));
		return -1;
	}
	if(pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED))
	{
		printf("pthread_attr_setdetachstate() failure: %s\n",strerror(errno));
		return -1;
	}

	pthread_create(&tid,&thread_attr,thread_worker1,&worker_zxp);
	printf("thread worker1 tid[%ld] create ok\n",tid);

	pthread_create(&tid,&thread_attr,thread_worker2,&worker_zxp);
	printf("thread worker2 tid[%ld] created ok\n",tid);

	//两个线程都设置了分离属性，这时主线程后面的while(1)就会执行了

	while(1)
	{
		printf("Main/control thread shared_var: %d\n",worker_zxp.shared_var);
		sleep(10);
	}
	pthread_mutex_destroy(&worker_zxp.lock);	//互斥锁在使用完毕之后，需要该函数来释放锁
}
void *thread_worker1(void *args)
{
	worker_zxp_t		*zxp = (worker_zxp_t*)args;

	if(!args)
	{
		printf("%s() get invalid arguments\n",__FUNCTION__);
		pthread_exit(NULL);
	}

	printf("thread worker1[%ld] start running...\n",pthread_self());

	while(1)
	{
		pthread_mutex_lock(&zxp -> lock);	//这里调用该函数来来申请锁，这里是阻塞锁，如过锁被别的线程持有，则该函数不会返回
		
		printf("+++: %s before shared_var++: %d\n",__FUNCTION__,zxp->shared_var);
		zxp->shared_var ++;
		sleep(2);
		printf("+++: %s after sleep shard_var: %d\n",__FUNCTION__,zxp->shared_var);

		pthread_mutex_unlock(&zxp->lock);	//在访问临界资源完毕之后，需要释放锁，使其它的线程能再次访问
		sleep(1);
	}
	printf("thread worker 1 exit...\n");
	
	return NULL;
}
void *thread_worker2(void *args)
{
	worker_zxp_t		*zxp = (worker_zxp_t*)args;

	if(!args)
	{
		printf("%s() get invalid arguments\n",__FUNCTION__);
		pthread_exit(NULL);
	
	}
	printf("thread worker 2 [%ld] start running...\n",pthread_self());

	while(1)
	{
		if(0 != pthread_mutex_trylock(&zxp->lock))
		{
			continue;
		}
		printf("---:%s before shared_var++:%d\n",__FUNCTION__,zxp->shared_var);
		zxp->shared_var++;
		sleep(2);
		printf("---:%s after sleep shared_var:%d\n",__FUNCTION__,zxp->shared_var);
		pthread_mutex_unlock(&zxp->lock);
		sleep(1);
	}

	printf("thread worker 2 exit...\n");
	
	return NULL;
}
