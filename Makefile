ifneq ($(KERNELRELEASE),)
	obj-m := scratchDriver.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	
all: default

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc dTest.c -o test
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

endif
