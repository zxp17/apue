#include <stdio.h>
#include <stdlib.h>

int main(int argc,char ** argv)
{
	char	buf[1024];
	char	*interface = "enp2s0";

	snprintf(buf,sizeof(buf),"ifconfig %s",interface);
	
	system(buf);

	return 0;
}
