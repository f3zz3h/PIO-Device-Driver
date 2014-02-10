/* 
usb 5-2: new full speed USB device using uhci_hcd and address 75
usb 5-2: New USB device found, idVendor=09ca, idProduct=5544
usb 5-2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
usb 5-2: Product: USB-PIO Digital I/O Module
usb 5-2: Manufacturer: BMC Messsysteme GmbH
usb 5-2: SerialNumber: 012531
usb 5-2: configuration #1 chosen from 1 choice
cdc_acm 5-2:1.0: ttyACM0: USB ACM device

*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>			/* kmalloc() */
#include <linux/usb.h>			/* USB stuff */
#include <linux/mutex.h>		/* mutexes */
#include <linux/ioctl.h>
#include <asm/uaccess.h>		/* copy_*_user */

#define PIO_VENDOR_ID	0x09CA
#define PIO_PRODUCT_ID	0x5544

struct usb_pio {

};

static struct usb_device_id pio_table [] = {
	{ USB_DEVICE(PIO_VENDOR_ID, PIO_PRODUCT_ID) },
	{ }
};

static int pio_open(struct inode *inode, struct file *file)
{
}

static int pio_release(struct inode *inode, struct file *file)
{
}

static ssize_t pio_write(struct file *file, const char __user *user_buf, size_t
		count, loff_t *ppos)
{
}


static struct file_operations pio_fops = {
	.owner =	THIS_MODULE,
	.write =	pio_write,
	.open =		pio_open,
	.release =	pio_release,
};

static int pio_probe(struct usb_interface *interface, const struct usb_device_id
		*id)
{
}

static void pio_disconnect(struct usb_interface *interface)
{
}

static struct usb_driver pio_driver = {
	.name = "usb_pio",
	.id_table = pio_table,
	.probe = pio_probe,
	.disconnect = pio_disconnect,
};

/* called on module loading */
static int __init usb_pio_init(void)
{

	
}

  /* called on module unloading */
static void __exit usb_pio_exit(void)
{
	
}

module_init(usb_pio_init);
module_exit(usb_pio_exit);

MODULE_AUTHOR("Luke, Matt, Seb & Joe");
MODULE_LICENSE("GPL");
