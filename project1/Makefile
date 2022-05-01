#定义变量指定生成的静态库和动态库的名字

LIBNAME=myserver

#定义一个变量指定库文件和头文件的安装路径

INSTPATH=`pwd`/../lib2

#定义编译器，如果需要交叉编译的话，只需要在这里改成交叉编译器即可

CC=gcc
AR=ar

#all是整个makefile的第一个目标，也就是总的目标，当我们输入make命令时就是要完成这个目标，该目标有两个依赖dynamic_lib和static_lib 和两个动作make clear和make intsall

all: dynamic_lib static_lib
	@make clear
	@make install

#dynamic_lib目标用来编译生成动态库

dynamic_lib:
	${CC} -shared -fPIC *.c -o lib2{LIBNAME}.so

#static_lib 目标用来编译生成动态库静态库

static_lib:
	${CC} -c *.c
	${AR} -rcs lib2${LIBNAME}.a *.o

#install是一个单独的目标，用来将编译生成的库文件和头文件拷贝到相应的安装路径下，在总目标下有该动作

install:
	cp -rf lib2${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

#uninstall是一个单独的目标，用来删除之前安装的库文件和头文件，想要执行时在命令行输入：make uninstall

uninstall:
	rm -f ${INSTPATH}/lib2{LIBNAME}.*
	rm -f ${INSTPATH}/*.h

#clear用来将编译生成的object临时文件删除，总目标下会执行该目标

clear:
	rm -f *.o

#clean 用来删除编译产生的库文件，先是依赖删除临时文件，执行时在命令行输入：make clean

clean: clear
	rm -f lib2${LIBNAME}.*

