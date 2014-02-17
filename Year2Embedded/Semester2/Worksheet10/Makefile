# makefile for loadable modules for 2.4.N kernels
# Remember the Target can be overridden at make time, e.g.
#            make TARGET=newProg
# To install the module use:
#            insmod path/TARGET.o
# To remove the module use:
#            rmmod TARGET
# where TARGET is the target module name e.g. hello-1
# and path is the module dir, e.g. .  (dot) for current dir
#
TARGET := hello-1
# some standard switches for Kernel Module compilation
WARN     := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
CFLAGS   := -O2 -DMODULE -D__KERNEL__ 
ASFLAGS  := -fverbose-asm -S

# Change the INCLUDE path to that of the linux being used
INCLUDE  := -isystem /home/netlab/jellis/Embedded/Semester2/Worksheet6/linux/include

# switches to be used for the ARM cross-compilation
ARMFLAGS := -fvolatile -mshort-load-words -mtune=strongarm -march=armv4
CC      := arm-linux-gcc

# Create the Kernel Object module. Hence use of –c flag!
$(TARGET).o : $(TARGET).c
	$(CC) $(WARN) $(CFLAGS) $(ARMFLAGS) $(INCLUDE) -c $(TARGET).c

.PHONY: clean

clean:
	rm -rf $(TARGET).o
