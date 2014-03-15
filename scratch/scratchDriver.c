
//usb driver written from scratch following ML skeleton code

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/slab.h> //kalloc
#include <linux/usb.h> //usb stuff :D
#include <linux/mutex.h> //mutexes
#include <linux/ioctl.h>

#include <asm/uaccess.h> // copy_*_user


#define VENDOR_ID 0x09ca
#define PRODUCT_ID 0x5544
#define PIO_STOP 0x1

#define INTERRUPT_ENDPOINT_ADDRESS 0X83
#define BULK_ENDPOINT_ADDRESS_IN 0X82
#define BULK_ENDPOINT_ADDRESS_OUT 0x01

#ifdef CONFIG_USB_DYNAMIC_MINORS
#define PIO_MINOR_BASE 0
#else
#define PIO_MINOR_BASE 15
#endif

struct usb_pio {
  struct usb_device *udev;
  struct usb_interface *interface;
  unsigned char minor;
  char unsigned serial_number[8];
  
  int open_count;
  struct semaphore sem;
  spinlock_t cmd_spinlock;

  char *int_in_buffer;
  struct usb_endpoint_descriptor *int_in_endpoint;
  struct urb *int_in_urb;
  int int_in_running;
 
  char  *bulk_in_buffer;
  struct usb_endpoint_descriptor *bulk_in_endpoint;
  struct urb *bulk_in_urb;
  int bulk_in_running;

  char  *bulk_out_buffer;
  struct usb_endpoint_descriptor *bulk_out_endpoint;
  struct urb *bulk_out_urb;
  int bulk_out_running;

  char *ctrl_buffer;
  struct urb *ctrl_urb;
  struct usb_ctrlrequest *ctrl_dr;
  int correction_required;

  __u8 command;
};

static struct usb_device_id pio_id_table [] = {
              { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },{}
              };	

MODULE_DEVICE_TABLE (usb, pio_id_table);

static DEFINE_MUTEX(disconnect_mutex);
static struct usb_driver usb_pio_driver;

static void pio_abort_transfers(struct usb_pio *dev)
{
  if (!dev)
  {
    printk("Dev is NULL\n");
    return;
  }

  if (!dev->udev)
  {
    printk("UDEV is NULL\n");
    return;
  }

  if (dev->udev->state == USB_STATE_NOTATTACHED)
  {
    printk("udev is not attatched!\n");
    return;
  }

  //shutdown transfer

  if (dev->int_in_running)
  {
    dev->int_in_running = 0;
    mb();
    if (dev->int_in_urb)
    {
       usb_kill_urb(dev->int_in_urb);
    }
  }
  if (dev->ctrl_urb)
  {
    usb_kill_urb(dev->ctrl_urb);
  }
  
}

static void pio_delete (struct usb_pio *dev)
{
  pio_abort_transfers(dev);
  
  if (dev->int_in_urb)
  {
    usb_free_urb(dev->int_in_urb);
  }

  if (dev->ctrl_urb)
  {
    usb_free_urb(dev->ctrl_urb);
  }

  kfree(dev->int_in_buffer);
  kfree(dev->ctrl_buffer);
  kfree(dev->ctrl_dr);
  kfree(dev);
}

static int pio_open (struct inode *inode, struct file *file)
{

  
  return 0;
}

static int pio_release (struct inode *inode, struct file *file)
{
  return 0;
}


static ssize_t pio_write (struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
  return 0;
}

static struct file_operations pio_fops = {
  .owner = THIS_MODULE,
  .write = pio_write,
  .open = pio_open,
  .release = pio_release,
};

static struct usb_class_driver pio_class = {
  .name = "pio%d",
  .fops = &pio_fops,
  .minor_base = PIO_MINOR_BASE,
};


static char* initialise_urb_buffer (int end_point_size, int* buf_err)
{
  char* buffer = kmalloc(end_point_size,GFP_KERNEL);
  if (! buffer)
  {
    buf_err = buf_err + 1;
    return NULL;
  }
  return buffer;
  
}

static struct urb* initialise_urb(int* urb_err)
{
  struct urb* init_urb = usb_alloc_urb(0,GFP_KERNEL);
  if (! init_urb)
  {
    urb_err = urb_err + 1;
  }    
  return init_urb;
  
  
}

static struct usb_endpoint_descriptor *set_endpoint(struct usb_endpoint_descriptor *endpoint, int endpoint_type, int endpoint_direction)
{
	if   (((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == endpoint_direction)
	      && ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == endpoint_type))
	{
	  printk(KERN_INFO KBUILD_MODNAME": interrup endpoint found!\n");
	  return endpoint;
	}
<<<<<<< Updated upstream
	return NULL;
=======
        return NULL;
>>>>>>> Stashed changes
}
static int pio_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  struct usb_device *udev = interface_to_usbdev(interface);
  struct usb_pio *dev = NULL;
  struct usb_host_interface *iface_desc;
  struct usb_endpoint_descriptor *endpoint;
  int i, endpoint_size, buffer_err = 0, urb_err = 0;
  int retval = -ENODEV;

  int int_flag = 0, bulk_flag_in = 0, bulk_flag_out = 0;

  if (! udev)
  {
    //DBG_ERR("udev is NULL");
    printk(KERN_INFO KBUILD_MODNAME": udev is null");
    goto exit;
  }


  dev = kzalloc(sizeof(struct usb_pio), GFP_KERNEL);

  if (! dev)
  {
    //DBG_ERR("cannot allocate memory for struct usb_pio");
    printk(KERN_INFO KBUILD_MODNAME": Cannot allocate memory for struct usb_pio");
    retval = -ENOMEM;
    goto exit;
  }

  dev->command = PIO_STOP;

  sema_init(&dev->sem,1);
  spin_lock_init(&dev->cmd_spinlock);

  dev->udev = udev;
  dev->interface = interface;
  iface_desc = interface->cur_altsetting;
  
  printk(KERN_INFO KBUILD_MODNAME ": %d endpoints found", iface_desc->desc.bNumEndpoints);

  for(i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
  {
    endpoint = &iface_desc->endpoint[i].desc;
    printk(KERN_INFO KBUILD_MODNAME ": Endpoint address %x, endpoint mask %d\n", endpoint->bEndpointAddress, USB_ENDPOINT_DIR_MASK);
    if (endpoint->bEndpointAddress == INTERRUPT_ENDPOINT_ADDRESS)
    { 
    	dev->int_in_endpoint = set_endpoint(endpoint, USB_ENDPOINT_XFER_INT, USB_DIR_IN);
    	int_flag = 1;
        endpoint_size = le16_to_cpu(dev->int_in_endpoint->wMaxPacketSize);
        dev->int_in_buffer = initialise_urb_buffer(endpoint_size, &buffer_err);
        dev->int_in_urb = initialise_urb(&urb_err);
    }
    else if (endpoint->bEndpointAddress == BULK_ENDPOINT_ADDRESS_IN)
    {
    	dev->bulk_in_endpoint = set_endpoint(endpoint, USB_ENDPOINT_XFER_BULK, USB_DIR_IN);
    	bulk_flag_in = 1;
        endpoint_size = le16_to_cpu(dev->bulk_in_endpoint->wMaxPacketSize);
        dev->bulk_in_buffer = initialise_urb_buffer(endpoint_size, &buffer_err);    
        dev->bulk_in_urb = initialise_urb(&urb_err);    
    }
    else if ((endpoint->bEndpointAddress == BULK_ENDPOINT_ADDRESS_OUT))
    {
    	dev->bulk_out_endpoint = set_endpoint(endpoint, USB_ENDPOINT_XFER_BULK, USB_DIR_OUT);
    	bulk_flag_out = 1;
        endpoint_size = le16_to_cpu(dev->bulk_out_endpoint->wMaxPacketSize);
        dev->bulk_out_buffer = initialise_urb_buffer(endpoint_size, &buffer_err);
        dev->bulk_out_urb = initialise_urb(&urb_err);        
    }
  }
  //printk(KERN_INFO KBUILD_MODNAME": buf_err = %d, urb_err = %d ----------\n",buffer_err, urb_err);
	if ((! dev->int_in_endpoint) && (int_flag))
	{
		printk(KERN_INFO KBUILD_MODNAME": could not find interupt in endpoint");
		//DBG_ERR("could not find interupt endpoint");
		goto error;
	}
	if ((! dev->bulk_in_endpoint)&& (bulk_flag_in))
	{
		printk(KERN_INFO KBUILD_MODNAME": could not find bulk in endpoint");
		//DBG_ERR("could not find interupt endpoint");
		goto error;
	}
	if ((! dev->bulk_out_endpoint)&& (bulk_flag_out))
	{
		printk(KERN_INFO KBUILD_MODNAME": could not find bulk out endpoint");
		//DBG_ERR("could not find interupt endpoint");
		goto error;
	}
<<<<<<< Updated upstream

  /* ..... */
  usb_set_intfdata(interface, dev);

=======
//setup control line
  dev->ctrl_urb = initialise_urb(&urb_err);
  dev->ctrl_buffer = initialise_urb_buffer(PIO_CTRL_BUFFER_SIZE, &buffer_err);
  dev->ctrl_dr = initialise_urb_buffer(sizeof (struct usb_ctrlrequest ), &buffer_err);
  if (buffer_err)
    {
      retval = -ENOMEM;
      goto error;
    }
  if (urb_err)
    {
      retval = -ENOMEM;
      goto error;
    }
/*//not ready for the stage, still shy
  dev->ctrl_dr->bRequestType = PIO_CTRL_REQUEST_TYPE;
  dev->ctrl_dr->bResquest = PIO_CTRL_REQUEST;
  dev->ctrl_dr->wValue = cpu_to_le16(PIO_CTRL_VALUE);
  dev->ctrl_dr->wIndex = cpu_to_le16(PIO_CTRL_INDEX);
  dev->ctrl_dr->wLength = cpu_to_le16(PIO_CTRL_BUFFER_SIZE);

  usb_fill_control_urb(dev->ctrl_urb, dev->udev, 
                             usb_sndctrlpipe(dev->udev,0), 
                            (unsigned char *)dev->ctrl_dr, 
                            dev->ctrl_buffer,
                            PIO_CTRL_BUFFER_SIZE,
                            pio_ctrl_callback,
                            dev);

  usb_set_intfdata(interface, dev);
*/
>>>>>>> Stashed changes
  /* We can register the device now, as it is ready */
  retval = usb_register_dev(interface, &pio_class);

  /* .......... */
//    printk(KERN_INFO KBUILD_MODNAME": something anyhting!!\n");
    

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

	printk("-------DC-------\n");
	mutex_lock(&disconnect_mutex);	/* Not interruptible */

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	//down(&dev->sem); /* Not interruptible */

	minor = dev->minor;
	/* Give back our minor. */

	usb_deregister_dev(interface, &pio_class);

	/* If the device is not opened, then we clean up right now. */
	if(!dev)
		printk("Dev is null\n\n");

	kfree(dev);

	mutex_unlock(&disconnect_mutex);

	printk("USB-PIO /dev/pio%d now disconnected\n", minor - PIO_MINOR_BASE);

}

static struct usb_driver usb_pio_driver = {
  .name = "usb_pio",
  .id_table = pio_id_table,
  .probe = pio_probe,
  .disconnect = pio_disconnect,
};

static int __init usb_pio_init(void)
{
  int result;
  printk(KERN_INFO KBUILD_MODNAME": Hello let us try and register this driver thingy!\n");
  result = usb_register(&usb_pio_driver);
  if (result)
  {
    printk(KERN_INFO KBUILD_MODNAME": Driver not Registered, Return val = %d\n",result);
  }
  else
  {
    printk(KERN_INFO KBUILD_MODNAME": Driver Registered\n");
  }
  return 0;
}

static void __exit usb_pio_exit(void)
{
  printk(KERN_INFO KBUILD_MODNAME": good bye!\n");
  usb_deregister(&usb_pio_driver);
  
}


module_init(usb_pio_init);
module_exit(usb_pio_exit);

MODULE_AUTHOR("loadsa people!!!");
MODULE_LICENSE("GPL");




