#!/bin/bash
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${LDFLAG} && \
	                ./mos_pub -a -i 5
