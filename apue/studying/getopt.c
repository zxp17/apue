#include <unistd.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	int ch;
	printf("\n\n");
	printf("现在选项的检索位置optind是: %d\n错误信息提示符opterr为: %d\n",optind,opterr);		//输出下一个检索的位置和“错误信息不要输出这一个提示符”
	printf("\n---------这是一个分界--------\n");

	while((ch = getopt(argc,argv,"ab:c:de::")) !=  -1)
	{
		printf("下一个选项的检索位置是: %d\n",optind);
		switch(ch)
		{
			case 'a':
				printf("have option: -a\n\n");
				break;

			case 'b':
				printf("have option: -b\n");
				printf("the argument of -b is %s\n\n",optarg);
				//如果有选项-b就输出提示信息，并输出该选项的参数
				break;

			case 'c':
				printf("have option: -c\n");
				printf("the argument of -c is %s\n\n",optarg);
				break;
				//如果有选项-c就输出提示信息，并输出该选项的参数

			case 'd':
				printf("have option: -d\n\n");
				break;

			case 'e':
				printf("have option -e\n");
				printf("the argument of -e is %s\n\n",optarg);
				break;
				//如果有选项-e就输出提示信息，并输出该选项的参数
			case '?':
				printf("unknown optio: %c\n",(char)optopt);
				break;
				//如果命令行中有规则中不存在的选项就输出提示信息

		}
	}
}
