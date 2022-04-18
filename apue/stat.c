#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
	struct stat *stbuf;
	struct stat buf;

	stbuf = &buf;

	//stbuf = malloc(sizeof(struct stat));

	stat("stat.c",stbuf);

	printf("File Mode :%o\n Real Size :%luB\n Space Size :%luB\n",stbuf->st_mode,stbuf->st_size,stbuf->st_blksize);

	return 0;
}
