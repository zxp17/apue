#include <stdio.h>
#include <stdlib.h>

int g_var1;	//g_var1是未初始化的全局变量，存放再数据段的BSS区，其值默认为0；
int g_var2 = 20;//g_var2是初始化了的全局变量，存放在数据段的DATA区，其值初始化为20；

int main(int argc,char **argv)	//argv里存放的是命令行参数，他存放在命令行参数区
{
	static int s_var1;	//s_var1是未初始化的静态变量，存放在数据段的BSS区，其值默认为0;
	static int s_var2 = 20;	//s_var2是初始化了的静态变量，存放在数据段的DATA区，其值初始化为20;
	char *str = "hello";	//str是初始化了的局部变量，存放在栈（stack）中，其值”hello“这个字符串常量存放在DATA段中RODATA（read only）区中的地址
	char *ptr;		//ptr是未初始化的局部变量，存放在栈中，其值为随机值，这时候的ptr称为野指针（未初始化的指针）
	ptr = malloc(10);	//malloc()会从堆（heap）中分配100个字节的内存空间，并将该内存空间的首地址返回给ptr存放

	printf("[cmd args]: argv address: %p\n",argv);
	printf("\n");

	printf("[stack]:  str address :%p\n",&str);
	printf("[stack]:  ptr address: %p\n",&ptr);
	printf("\n");

	printf("[heap]:  malloc address: %p\n",ptr);
	printf("\n");

	printf("[ bss ]: s_var1 address: %p value: %d\n",&s_var1,s_var1);
	printf("[ bss ]: g_var1 address: %p value: %d\n",&g_var1,g_var1);
	printf("\n");

	printf("[ data ]: g_var2 address: %p value: %d\n",&g_var2,g_var2);
	printf("[ data ]: s_var2 address: %p value: %d\n",&s_var2,s_var2);
	printf("\n");

	printf("[ rodata]: \"%s\" address :%p \n",str,str);
	printf("\n");

	return 0;

}
