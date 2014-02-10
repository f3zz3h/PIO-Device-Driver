#!/bin/sh
cdc="cdc_acm"
module="pio_driver"
devname="usb_device"
device="pio"
mode=664
group="wheel"

load()
{
	shift
	insmod ./${module}.ko $* || exit 1

	# Remove stale nodes
	rm -f /dev/${device}[0-3]

	major=$(awk "\$2==\"${devname}\" {print \$1}" /proc/devices)

	for i in 0 1 2 3; do
		mknod -m ${mode} "/dev/${device}${i}" c ${major} ${i}
		echo mknod -m ${mode} "/dev/${device}${i}" c ${major} ${i}

	done

	chgrp ${group} /dev/${device}[0-3]
}

unload()
{
	rmmod ${module} || exit 1
	rm -f /dev/${device}[0-3]
}
cdcunload()
{	
	rmmod ${cdc} || exit 1
}


case $1 in
	load)
	load $*
	;;
	unload)
	unload
	;;
	reload)
	unload
	load $*
	;;
	cdcunload)
	unload_cdc
	;;
	*)
	echo "usage: $0 <load|unload|reload>"
	;;
esac
