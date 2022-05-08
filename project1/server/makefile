#定义变量指定生成的静态库和动态库的名字

LIBNAME=myserver
APPNAME=server

#定义一个变量指定库文件和头文件的安装路径

INSTPATH=/home/zhengxuping/project1/server

#定义头文件和库文件的路径
I_PATH=/home/zhengxuping/project1/install/include
L_PATH=/home/zhengxuping/project1/install/lib

#定义编译器，如果需要交叉编译的话，只需要在这里改成交叉编译器即可

CC=gcc


#all是整个makefile的第一个目标，也就是总的目标，当我们输入make命令时就是要完成这个目标，该目标有三个依赖dynamic_lib和dynamic_link,run_server

all: dynamic_lib dynamic_link run_server
	

#dynamic_lib目标用来编译生成动态库

dynamic_lib:
#	${CC} -shared -fPIC logger.c -o lib${LIBNAME}.so -I ${I_PATH} -L ${L_PATH} -lsqlite3
	${CC} server.c -o ${APPNAME} -I ${I_PATH} -L ${L_PATH} -lsqlite3	

#dynamic_link用来链接
dynamic_link:
#	${CC} server.c -o ${APPNAME} -I ${INSTPATH} -l${LIBNAME} -I ${I_PATH} -L ${L_PATH} -lsqlite3 
	

#make server run
run_server:
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${L_PATH} && \
		./${APPNAME} -p 7777

#clean dynamic_lib
clean: 
	rm -f ${APPNAME}
	rm -f *.o

