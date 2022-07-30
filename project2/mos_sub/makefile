
LIBNAME=mysub
APPNAME=mos_sub

INSTPATH=./

CFLAGS=../inc/
LDFLAG=../lib/

CC=gcc

all:dynamic_lib dynamic_link run


#dynamic_lib目标用来编译生成动态库
dynamic_lib:
	${CC} -shared -fPIC cJSON.c dictionary.c  iniparser.c mosq_conf.c libgpiod-led.c -o lib${LIBNAME}.so -I ${CFLAGS} -L ${LDFLAG}  -lmosquitto -lgpiod

#dynamic_link用来链接
dynamic_link:
	cp lib${LIBNAME}.so ${LDFLAG}
	${CC} mos_sub.c -o ${APPNAME} -I ${CFLAGS} -L ${LDFLAG} -lmosquitto -l${LIBNAME}

#make process on
run:
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${LDFLAG} && \
		./${APPNAME} 

#clean dynamic_lib
clean:
	rm -f ${APPNAME}
	rm -f *.o
	rm -f *.so
