obj-m := cdc-acm.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -f modules.order cdc-acm.ko .cdc-acm.ko.cmd .cdc-acm.mod.o.cmd .cdc-acm.o.cmd Module.symvers cdc-acm.mod.c cdc-acm.mod.o cdc-acm.o
	rm -rf .tmp_versions

ins: cdc-acm.ko
	sudo insmod cdc-acm.ko

rmm: cdc-acm.ko
	sudo rmmod cdc-acm.ko

ls:
	@ls -l /dev/ | grep dio[0-9]*$
