#定义变量指定生成的静态库和动态库的名字

LIBNAME=myclient
APPNAME=client

#定义一个变量指定库文件和头文件的安装路径

INSTPATH=/home/zhengxuping/project1/client

#定义头文件和可文件的路径
I_PATH=/home/zhengxuping/project1/install/include
L_PATH=/home/zhengxuping/project1/install/lib

#定义编译器，如果今后交叉编译的话，只需要在这里改成交叉编译器即可

CC=gcc


#all是整个makefile文件的第一个目标，也叫总目标，当输入make命令时就是要完成这个目标；该目标有两个依赖dynamic_lib和static_lib;还有两个动作make clear和make install

all: dynamic_lib dynamic_link run_client
	

#dynamic_lib 目标用来编译生成动态库，它是all目标的一个依赖
dynamic_lib:
#	${CC} -shared -fPIC logger.c gettime.c gettemper.c client_database.c -o lib${LIBNAME}.so -I ${I_PATH} -L ${L_PATH} -lsqlite3
	${CC} client.c -o ${APPNAME} -I ${I_PATH} -L ${L_PATH} -lsqlite3

#dynamic_link 用来链接动态库
dynamic_link:
#	${CC} client.c -o ${APPNAME} -I ${I_PATH} -L ${L_PATH} -lsqlite3  

run_client:
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${L_PATH} && \
		./${APPNAME} -i 127.0.0.1 -p 7777 -s 60


clean: 
	rm -f ${APPNAME}
	rm -f *.o

