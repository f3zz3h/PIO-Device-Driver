/*
 * scratchDriver.h
 *
 *  Created on: 23 Apr 2014
 *      Author: luke
 */

#ifndef SCRATCHDRIVER_H_
#define SCRATCHDRIVER_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h> //kalloc
#include <linux/usb.h> //usb stuff :D
#include <linux/usb/cdc.h>
#include <linux/mutex.h> //mutexes
#include <linux/ioctl.h>
#include <asm/uaccess.h> // copy_*_user

#define VENDOR_ID 0x09ca
#define PRODUCT_ID 0x5544
#define PIO_STOP 0x1
#define INTERRUPT_ENDPOINT_ADDRESS 0X83
#define BULK_ENDPOINT_ADDRESS_IN 0X82
#define BULK_ENDPOINT_ADDRESS_OUT 0x01
#define CDC_DATA_INTERFACE_TYPE	0x0a


#define PIO_CTRL_BUFFER_SIZE 	8
#define PIO_CTRL_REQEUST_TYPE	0x21 //Pretty sure this is wrong??
#define PIO_CTRL_REQUEST		0x09
#define PIO_CTRL_VALUE			0x0
#define PIO_CTRL_INDEX			0x0

#ifdef CONFIG_USB_DYNAMIC_MINORS
	#define PIO_MINOR_BASE 0
#else
	#define PIO_MINOR_BASE 15
#endif

static struct usb_device_id pio_id_table[] =
{
		{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
		{ }
};
struct usb_pio
{
  struct usb_device *udev; //the control usb_device
  struct usb_device *data_dev; //the data usb device
  struct usb_interface *control_interface; //control interface - a signle INT endpoint
	struct usb_interface *data_interface; //data interface - holds rx and tx lines
	unsigned char minor; //minor number for /proc/dev
	char unsigned serial_number[8]; //number that comes with the device

	int open_count;
	struct semaphore sem;
	spinlock_t cmd_spinlock;

	char *int_in_buffer; //ctrl buffer
	struct usb_endpoint_descriptor *int_in_endpoint; //ctrl endpoint
	struct urb *int_in_urb; //ctrl urb
	int int_in_running; //??

	//tx
	char *bulk_in_buffer;
  char *bulk_in_ptr;
  unsigned int read_ready;
  unsigned int read_lock;
  spinlock_t bulk_in_lock;
	struct usb_endpoint_descriptor *bulk_in_endpoint;
	struct urb *bulk_in_urb;
	int bulk_in_running;

	//rx
	char *bulk_out_buffer;
	struct usb_endpoint_descriptor *bulk_out_endpoint;
	struct urb *bulk_out_urb;
	int bulk_out_running;

	char *ctrl_buffer;
	struct urb *ctrl_urb;
	struct usb_ctrlrequest *ctrl_dr;
	int correction_required;

	__u8 command;
};

MODULE_DEVICE_TABLE( usb, pio_id_table);

static DEFINE_MUTEX( disconnect_mutex);

static struct usb_driver usb_pio_driver;
static struct usb_pio* global_dev;

/* Function declerations */
static void pio_abort_transfers(struct usb_pio *dev);
static void pio_delete(struct usb_pio *dev);
static void pio_int_in_callback(struct urb *urb);
static long pio_ioctl(struct file *file, unsigned int cmd, unsigned long int arg);
static int pio_open(struct inode *inode, struct file *file);
static int pio_release(struct inode *inode, struct file *file);
static ssize_t pio_write (struct file *file, const char __user *user_buf, size_t count, loff_t *ppos);
static ssize_t pio_read (struct file *file, const char __user *user_buf, size_t count, loff_t *f_pos);
static struct urb* initialise_urb(int* urb_err);
static int pio_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void pio_disconnect(struct usb_interface *interface);
static int __init usb_pio_init(void);
static void __exit usb_pio_exit(void);

#endif /* SCRATCHDRIVER_H_ */
