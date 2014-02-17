PIO-Device-Driver
=================

A device driver for the USB PIO is a proof-of-concept Linux kernel driver for the USB-PIO. It is developed mainly to gain experience in kernel drivers. Hence it is mereley a skeleton at the moment, funtionality should be extended over it's lifetime.

============
Installation
============

Compilation is straigt forward:

    make

To unload the defualt cdc_acm driver before loading into the kernel

    ./pio_manage.sh cdcunload

To load the module

    ./pio_manage.sh unload

To unload when you are finished

    ./pio_manage.sh unload


=============
Source Website
=============

http://matthias.vallentin.net/blog/2007/04/writing-a-linux-kernel-driver-for-an-unknown-usb-device/
