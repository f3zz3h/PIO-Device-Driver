/*
 * Debugging 
 */
#define DEBUG_LEVEL_DEBUG	0x1F
#define DEBUG_LEVEL_INFO	0x0F
#define DEBUG_LEVEL_WARN	0x07
#define DEBUG_LEVEL_ERROR	0x03
#define DEBUG_LEVEL_CRITICAL	0x01

#define DBG_DEBUG(fmt, args...) \
if ((debug_level & DEBUG_LEVEL_DEBUG) == DEBUG_LEVEL_DEBUG) \
	printk( KERN_DEBUG "[debug] %s(%d): " fmt "\n", \
			__FUNCTION__, __LINE__, ## args)
#define DBG_INFO(fmt, args...) \
if ((debug_level & DEBUG_LEVEL_INFO) == DEBUG_LEVEL_INFO) \
	printk( KERN_DEBUG "[info]  %s(%d): " fmt "\n", \
			__FUNCTION__, __LINE__, ## args)
#define DBG_WARN(fmt, args...) \
if ((debug_level & DEBUG_LEVEL_WARN) == DEBUG_LEVEL_WARN) \
	printk( KERN_DEBUG "[warn]  %s(%d): " fmt "\n", \
			__FUNCTION__, __LINE__, ## args)
#define DBG_ERR(fmt, args...) \
if ((debug_level & DEBUG_LEVEL_ERROR) == DEBUG_LEVEL_ERROR) \
	printk( KERN_DEBUG "[err]   %s(%d): " fmt "\n", \
			__FUNCTION__, __LINE__, ## args)
#define DBG_CRIT(fmt, args...) \
if ((debug_level & DEBUG_LEVEL_CRITICAL) == DEBUG_LEVEL_CRITICAL) \
	printk( KERN_DEBUG "[crit]  %s(%d): " fmt "\n", \
			__FUNCTION__, __LINE__, ## args)


/* MUTEX fix */
#ifndef init_MUTEX
	#define init_MUTEX(x) sema_init(x, 1)
#endif

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

#ifdef CONFIG_USB_DYNAMIC_MINORS
	#define PIO_MINOR_BASE	0
#else
	#define PIO_MINOR_BASE	96
#endif

struct usb_pio {
	struct usb_device *udev;
	struct usb_interface *interface;
	unsigned char minor;
	char serial_number[8];
	int open_count;	/* Open count for this port */
	struct 	semaphore sem; /* Locks this structure */
	spinlock_t cmd_spinlock; /* locks dev->command */

	char *int_in_buffer;
	struct usb_endpoint_descriptor *int_in_endpoint;
	struct urb *int_in_urb;
	int int_in_running;

	char *ctrl_buffer;	/* 8 byte buffer for the control msg */
	struct urb *ctrl_urb;
	struct usb_ctrlrequest *ctrl_dr; /* Setup packet information */
	int correction_required;

	__u8 command;		/* Last issued command */
};

static struct usb_device_id pio_table [] = {
	{ USB_DEVICE(PIO_VENDOR_ID, PIO_PRODUCT_ID) },
	{ }
};



MODULE_DEVICE_TABLE (usb, pio_table);

static int debug_level = DEBUG_LEVEL_INFO;
static int debug_trace = 0;
module_param(debug_level, int, S_IRUGO | S_IWUSR);
module_param(debug_trace, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug_level, "debug level (bitmask)");
MODULE_PARM_DESC(debug_trace, "enable function tracing");

/* Prevent races between open() and disconnect */
static DEFINE_MUTEX(disconnect_mutex);
static struct usb_driver pio_driver;




static void pio_abort_transfers(struct usb_pio *dev)
{
	if (! dev) { 
	//	DBG_ERR("dev is NULL");
		return;
	}

	if (! dev->udev) {
	//	DBG_ERR("udev is NULL");
		return;
	}

	if (dev->udev->state == USB_STATE_NOTATTACHED) {
	//	DBG_ERR("udev not attached");
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

	return NULL; 
}


static int pio_open(struct inode *inode, struct file *file)
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



static struct file_operations pio_fops = {
	.owner =	THIS_MODULE,
	.write =	pio_write,
	.open =		pio_open,
	.release =	pio_release,
};

static struct usb_class_driver pio_class = {
	.name = "pio%d",
	.fops = &pio_fops,
	.minor_base = PIO_MINOR_BASE,
};

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

	//dev->command = PIO_STOP;

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

static struct usb_driver pio_driver = {
	.name = "usb_pio",
	.id_table = pio_table,
	.probe = pio_probe,
	.disconnect = pio_disconnect,
};

/* called on module loading */
static int __init usb_pio_init(void)
{
	usb_register(&pio_driver);
}

  /* called on module unloading */
static void __exit usb_pio_exit(void)
{
	usb_deregister(&pio_driver);
	DBG_INFO("module deregistered");	
}

module_init(usb_pio_init);
module_exit(usb_pio_exit);

MODULE_AUTHOR("Luke, Matt, Seb & Joe");
MODULE_LICENSE("GPL");
