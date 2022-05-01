#定义变量指定生成的静态库和动态库的名字

LIBNAME=myclient


#定义一个变量指定库文件和头文件的安装路径

INSTPATH=`pwd`/../lib1/


#定义编译器，如果今后交叉编译的话，只需要在这里改成交叉编译器即可

CC=gcc
AR=ar


#all是整个makefile文件的第一个目标，也叫总目标，当输入make命令时就是要完成这个目标；该目标有两个依赖dynamic_lib和static_lib;还有两个动作make clear和make install

all: dynamic_lib static_lib 
	@make clear
	@make install

#dynamic_lib 目标用来编译生成动态库，它是all目标的一个依赖

dynamic_lib:
	${CC} -shared -fPIC *.c -o lib1${LIBNAME}.so


#static_lib 目标用来编译生成静态库，它是all目标的一个依赖

static_lib:
	${CC} -c *.c
	${AR} -rcs lib1${LIBNAME}.a *.o

#install 是一个单独的目标，用来编译生成的库文件和头文件拷贝到相应的安装路径下，总目标all下有一个动作会执行该目标

install:
	cp -rf lib1${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

#uninstall是一个单独的目标，用来在安装路径下删除之前安装的库文件和头文件，想要执行该命令是在命令行下输入make uninstall

uninstall:
	rm -f ${INSTPATH}/lib1${LIBNAME}.*
	rm -f ${INSTPATH}/*.h

#clear是一个单独的目标，用来将编译生成的object临时文件删除，总目标下有个动作会执行该目标

clear:
	rm -f *.o

#clean是一个单独的目标，它依赖clear目标，所以先通过clear目标删除object临时文件，之后再删除编译生成的库文件，想执行该目标的时候，在命令行下输入make clean

clean: clear
	rm -f lib1${LIBNAME}.*

