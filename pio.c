#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>			/* kmalloc() */
#include <linux/usb.h>			/* USB stuff */
#include <linux/mutex.h>		/* mutexes */
#include <linux/ioctl.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <asm/uaccess.h>		/* copy_*_user */
#include <linux/kernel.h>

#include "pio.h"

/* Not sure we need the below

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/usb.h>
#include <linux/usb/cdc.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <linux/list.h>
*/

/* 
usb 5-2: new full speed USB device using uhci_hcd and address 75
usb 5-2: New USB device found, idVendor=09ca, idProduct=5544
usb 5-2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
usb 5-2: Product: USB-PIO Digital I/O Module
usb 5-2: Manufacturer: BMC Messsysteme GmbH
usb 5-2: SerialNumber: 012531
usb 5-2: configuration #1 chosen from 1 choice
cdc_acm 5-2:1.0: ttyACM0: USB ACM device


cat /proc/tty/drivers
acm   /dev/ttyACM   166 0-31 serial

tree /sys/class/tty
ttyACM0 -> ../../devices/pci0000:00/0000:00:1d.0/usb6/6-2/6-2:1.0/tty/ttyACM0

cat /sys/class/tty/ttyACM0/dev
166:0

*/


static void pio_abort_transfers(struct usb_pio *dev)
{
	if (! dev) { 
		DBG_ERR("dev is NULL");
		return;
	}

	if (! dev->udev) {
		DBG_ERR("udev is NULL");
		return;
	}

	if (dev->udev->state == USB_STATE_NOTATTACHED) {
		DBG_ERR("udev not attached");
		return;
	}

	/* Shutdown transfer */
	if (dev->int_in_running) {
		dev->int_in_running = 0;
		mb();
		if (dev->int_in_urb)
			usb_kill_urb(dev->int_in_urb);
	}

	if (dev->ctrl_urb)
		usb_kill_urb(dev->ctrl_urb);
}

static inline void pio_delete(struct usb_pio *dev)
{
	pio_abort_transfers(dev);

	/* Free data structures. */
	if (dev->int_in_urb)
		usb_free_urb(dev->int_in_urb);
	if (dev->ctrl_urb)
		usb_free_urb(dev->ctrl_urb);

	kfree(dev->int_in_buffer);
	kfree(dev->ctrl_buffer);
	kfree(dev->ctrl_dr);
	kfree(dev);
}

static void pio_ctrl_callback(struct urb *urb)
{
	struct usb_pio *dev = urb->context;
	dev->correction_required = 0;
}

/* Use to stop device breaking it self i.e stepper motor with a limit */
static void pio_int_in_callback(struct urb *urb)
{
	struct usb_pio *dev = urb->context;
	int retval;
}


static int pio_open(struct inode *inode, struct file *file)
{
	return NULL;
}
static int pio_close(struct inode *inode, struct file *file)
{
	return NULL;
}
static int pio_set_termios(struct inode *inode, struct file *file)
{
	return NULL;
}
static int pio_write_room(struct inode *inode, struct file *file)
{
	return NULL;
}



static int pio_release(struct inode *inode, struct file *file)
{
	return NULL;
}

static ssize_t pio_write(struct file *file, const char __user *user_buf, size_t
		count, loff_t *ppos)
{
	return NULL;
}


static int pio_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_pio *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i, int_end_size;
	int retval = -ENODEV;

	if (! udev)
	{
		//DBG_ERR("udev is NULL");
		goto exit;
	}

	dev = kzalloc(sizeof(struct usb_pio), GFP_KERNEL);
	
	if (! dev)
	{
		//DBG_ERR("cannot allocate memory for struct");
		retval = -ENOMEM;
		goto exit;
	}

	init_MUTEX(&dev->sem);
	spin_lock_init(&dev->cmd_spinlock);

	dev->udev = udev;
	dev->interface = interface;
	iface_desc = interface->cur_altsetting;

	/* Set up interrupt endpoint information. */
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
	{
		endpoint = &iface_desc->endpoint[i].desc;

		if (((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)
			&& ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
			    USB_ENDPOINT_XFER_INT))
		    dev->int_in_endpoint = endpoint;

	}
	
	if (! dev->int_in_endpoint)
	{
		//DBG_ERR("could not find interrupt in endpoint");
		goto error;
	}

	/* ... */

	/* We can register the device now, as it is ready. */
	retval = usb_register_dev(interface, &pio_class);

	/* ... */

error:
	pio_delete(dev);
	return retval;
exit:
	return retval;

}

static void pio_disconnect(struct usb_interface *interface)
{
	struct usb_pio *dev;
	int minor;

	mutex_lock(&disconnect_mutex);	/* Not interruptible */

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	down(&dev->sem); /* Not interruptible */

	minor = dev->minor;

	/* Give back our minor. */
	usb_deregister_dev(interface, &pio_class);

	/* If the device is not opened, then we clean up right now. */
	if (! dev->open_count) {
		up(&dev->sem);
		pio_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	mutex_unlock(&disconnect_mutex);

	//DBG_INFO("USB-PIO /dev/ttyACM%d now disconnected", minor - PIO_MINOR_BASE);
}


/* called on module loading 
	currently a copy of cdc-acm needs some changes adding
*/
static int __init usb_pio_init(void)
{
	int retval;
	pio_tty_driver = alloc_tty_driver(ACM_TTY_MINORS);
	if (!pio_tty_driver)
		return -ENOMEM;
	pio_tty_driver->owner = THIS_MODULE,
	pio_tty_driver->driver_name = "acm",
	pio_tty_driver->name = "ttyACM",
	pio_tty_driver->major = ACM_TTY_MAJOR,
	pio_tty_driver->minor_start = 0,
	pio_tty_driver->type = TTY_DRIVER_TYPE_SERIAL,
	pio_tty_driver->subtype = SERIAL_TYPE_NORMAL,
	pio_tty_driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
	pio_tty_driver->init_termios = tty_std_termios;
	pio_tty_driver->init_termios.c_cflag = B9600 | CS8 | CREAD |
								HUPCL | CLOCAL;
	tty_set_operations(pio_tty_driver, &pio_ops);

	retval = tty_register_driver(pio_tty_driver);
	if (retval) {
		put_tty_driver(pio_tty_driver);
		return retval;
	}

	retval = usb_register(&pio_driver);
	if (retval) {
		tty_unregister_driver(pio_tty_driver);
		put_tty_driver(pio_tty_driver);
		return retval;
	}

	printk(KERN_INFO KBUILD_MODNAME ": Driver version could go here! \n");

	return 0;

}

/* called on module unloading - COMPLETED 17/04/2014 - Luke Hart*/
static void __exit usb_pio_exit(void)
{
	usb_deregister(&pio_driver);
	tty_unregister_driver(pio_tty_driver);
	put_tty_driver(pio_tty_driver);	
	DBG_INFO("Module deregistered");
}

module_init(usb_pio_init);
module_exit(usb_pio_exit);

MODULE_AUTHOR("Luke, Matt, Seb & Joe");
MODULE_LICENSE("GPL");
