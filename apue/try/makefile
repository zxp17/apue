#定义变量指定生成的静态库和动态库的名字
LIBNAME=mycrypto

#定义一个变量指定库文件和头文件的安装路径

INSTPATH=`pwd`/../lib/

#定义编译器，如果今后交叉编译的话，只需要在这里改成交叉编译器即可

CC=gcc
AR=ar

#这里的all是整个makefile文件的第一个目标，也是总的目标，当我们输入make命令时就是要完成这个目标，改目标有两个依赖dynamic_lib和static_lib和两个动作 make clear 和make install

#其中两个依赖是makefile文件的目标，所以整个makefile要先执行完这两个目标后才能执行后面的动作

#两个动作前面有个@符号，这个符号会让执行make命令是不打印这两个命令本身，而只是输出命令执行的结果，另外make命令在执行过程中，也会多次载入makefile文件

all:dynamic_lib static_lib
	@make clear
	@make install

#dynamic_lib目标用来编译生成动态库，它是all目标的一个依赖；

dynamic_lib:
	${CC} -shared -fPIC *.c -o lib${LIBNAME}.so

#static_lib 目标用来编译生成静态库，它是all目标的一个依赖

static_lib:
	${CC} -c *.c
	${AR} -rcs lib${LIBNAME}.a *.o

#install是一个单独的目标，他用来将编译生成的库文件和头文件拷贝到相应的安装路径下，在总目标all下有个动作@make install 会执行这个目标

install:
	cp -rf lib${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

#uninstall是一个单独的目标，他用来在安装路径下删除之前安装的库文件和头文件
#该目标没有北总的目标all依赖或执行，所以默认该目标不会被执行，如果想执行该目标，可在linux命令行下输入make uninstall

uninstall:
	rm -f ${INSTPATH}/lib${LIBNAME}.*
	rm -f ${INSTPATH}/*.h

#clear是一个单独的目标，他用来将编译生成的object临时文件删除，在总目录all下有个动作@make clear会执行该目标；

clear:
	rm -f *.o

#clean是一个单独的目标，它依赖clear目标，所以先通过clear目标删除所有的object临时文件，之后再删除编译文件产生的库文件
#该目标没有被总的目标all依赖或窒息你个，所有默认该目标不会被执行，如果相应执行该目标，则可以在linux命令行下输入make clean

clean:clear
	rm -f lib${LIBNAME}.*
