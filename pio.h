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

/* Major & Minor Numbers */
#define ACM_TTY_MAJOR		166
#define ACM_TTY_MINORS		32

#define PIO_VENDOR_ID	0x09CA
#define PIO_PRODUCT_ID	0x5544

/* Globals Vars */

MODULE_DEVICE_TABLE (usb, pio_table);

static int debug_level = DEBUG_LEVEL_INFO;
static int debug_trace = 0;
module_param(debug_level, int, S_IRUGO | S_IWUSR);
module_param(debug_trace, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug_level, "debug level (bitmask)");
MODULE_PARM_DESC(debug_trace, "enable function tracing");

/* Prevent races between open() and disconnect */
static DEFINE_MUTEX(disconnect_mutex);


/* Structs */
struct usb_pio {
	struct usb_device 	*udev;
	struct usb_interface *interface;
	unsigned char 		minor;
	char				serial_number[8];

	int					open_count;		/* Open count for this port */
	struct 				semaphore sem;	/* Locks this structure */
	spinlock_t			cmd_spinlock;	/* locks dev->command */

	char 				*int_in_buffer;
	struct usb_endpoint_descriptor *int_in_endpoint;
	struct urb 			*int_in_urb;
	int					int_in_running;

	char				*ctrl_buffer;	/* 8 byte buffer for the control msg */
	struct urb			*ctrl_urb;
	struct usb_ctrlrequest *ctrl_dr;	/* Setup packet information */
	int					correction_required;

	__u8				command;		/* Last issued command */
};


static struct usb_device_id pio_table[] = {
	{ USB_DEVICE(PIO_VENDOR_ID, PIO_PRODUCT_ID) },
	{ }
};

static struct tty_driver *pio_tty_driver;

/* Function declareations */
static void pio_abort_transfers(struct usb_pio *dev);
static inline void pio_delete(struct usb_pio *dev);
static void pio_ctrl_callback(struct urb *urb);
static void pio_int_in_callback(struct urb *urb);
static int pio_open(struct inode *inode, struct file *file);
static int pio_release(struct inode *inode, struct file *file);
static ssize_t pio_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos);
static int pio_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void pio_disconnect(struct usb_interface *interface);
static int pio_close(struct inode *inode, struct file *file);
static int pio_set_termios(struct inode *inode, struct file *file);
static int pio_write_room(struct inode *inode, struct file *file);


/* Structs that require functions */
static struct usb_driver pio_driver = {
	.name = "usb_pio", //cdc-acm
	.id_table = pio_table,
	.probe = pio_probe,
	.disconnect = pio_disconnect,
};

static struct file_operations pio_fops = {
	.owner =	THIS_MODULE,
	.write =	pio_write,
	.open =		pio_open,
	.release =	pio_release,
};

static struct usb_class_driver pio_class = {
	.name = "pio%d",
	.fops = &pio_fops,
	//.minor_base = PIO_MINOR_BASE,
};

/*
 * TTY driver structures.
 */
static const struct tty_operations pio_ops = {
	.open = pio_open,
	.close = pio_close,
	.write = pio_write,
	.write_room = pio_write_room,
	.set_termios = pio_set_termios,
};
