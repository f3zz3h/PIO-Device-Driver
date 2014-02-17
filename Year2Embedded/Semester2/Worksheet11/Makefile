#makefile for loadable modules.  2.4.N kernels

TARGET	:= fpga_dio
WARN	:= -W -Wall -Wstrict-prototypes -Wmissing-prototypes -fvolatile -mshort-load-words
INCLUDE	:= -isystem /home/netlab/jellis/Embedded/Semester2/Worksheet6/linux/include
CFLAGS	:= -O2 -DMODULE -D__KERNEL__  -mtune=strongarm -march=armv4 ${WARN} ${INCLUDE}
ASFLAGS 	:= -fverbose-asm -S
CC		:= arm-linux-gcc


${TARGET}.o : ${TARGET}.c 

.PHONY: clean

clean:
	rm -rf ${TARGET}.o
