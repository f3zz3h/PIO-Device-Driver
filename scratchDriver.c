
//usb driver written from scratch following ML skeleton code

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

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

#ifdef CONFIG_USB_DYNAMIC_MINORS
#define PIO_MINOR_BASE 0
#else
#define PIO_MINOR_BASE 15
#endif

struct usb_pio {
  struct usb_device *udev;                    //the usb_device
  struct usb_interface *control_interface;   //control interface - a signle INT endpoint
  struct usb_interface *data_interface;      //data interface - holds rx and tx lines
  unsigned char minor;                       //minor number for /proc/dev
  char unsigned serial_number[8];            //number that comes with the device
  
  int open_count;
  struct semaphore sem;
  spinlock_t cmd_spinlock;

  char *int_in_buffer;                       //ctrl buffer
  struct usb_endpoint_descriptor *int_in_endpoint;  //ctrl endpoint
  struct urb *int_in_urb;                    //ctrl urb
  int int_in_running;                        //??
 
  //rx
  char  *bulk_in_buffer;                     
  struct usb_endpoint_descriptor *bulk_in_endpoint;
  struct urb *bulk_in_urb; 
  int bulk_in_running;
  
  //rx
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
static struct usb_pio* global_dev;

static void pio_abort_transfers(struct usb_pio *dev)
{
  if (!dev)
  {
    printk(" Abort_transfers Dev is NULL\n");
    return;
  }

  if (!dev->udev)
  {
    printk(" Abort_transfers - UDEV is NULL\n");
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


static void pio_int_in_callback(struct urb *urb)
{
  struct usb_pio *dev = urb->context;
  int retval;
  printk ("----Completion handler----contest=%d\n", urb->status);
  
  if (urb->status&& !(urb->status == -ENOENT || 
		      urb->status == -ECONNRESET ||
		      urb->status == -ESHUTDOWN))
    {
       printk("No zero status recieved\n status ");		   
    }
  
  	
  usb_free_coherent(urb->dev, urb->transfer_buffer_length, 
		    urb->transfer_buffer, urb->transfer_dma);
  
  /*		
		resubmit:
		// Resubmit if we're still running. 
		if (dev->int_in_running && dev->udev)
		{
		retval = usb_submit_urb(urb, GFP_ATOMIC);
		if (retval)
		{
		printk(KERN_INFO KBUILD_MODNAME "resubmitting urb failed (%d)", retval);
		dev->int_in_running = 0;
		}
	}
	*/
	
}
static int pio_open (struct inode *inode, struct file *file)
{
  struct usb_pio *dev = NULL;
  struct usb_interface *interface;
  int subminor;
  int retval = 0;
  char buffer[32] = {'@','0','0','P','2','3','F','\r'};
  int i;
  subminor = iminor(inode);

  //mutex_lock(&disconnect_mutex);
  interface = usb_find_interface(&usb_pio_driver, subminor);
 printk(KERN_INFO KBUILD_MODNAME " Checking Global Dev!\n");
  if (global_dev == NULL)
    {
      printk(KERN_INFO KBUILD_MODNAME " global_dev == NULL!\n");
      retval = -ENODEV;
      goto exit;
    }
 printk(KERN_INFO KBUILD_MODNAME " Checking global dev data interface\n");
  if (global_dev->data_interface == NULL)
    {
      printk(KERN_INFO KBUILD_MODNAME " Data_interface == NULL!\n");
      retval = -ENODEV;
      goto exit;
    }
 printk(KERN_INFO KBUILD_MODNAME "Checking global_dev->control_interface!\n");
  if (global_dev->control_interface == NULL)
    {
      printk(KERN_INFO KBUILD_MODNAME "control_interface == NULL!\n");
      goto exit;
    } 
 printk(KERN_INFO KBUILD_MODNAME "Checking gloval UDEV\n");
 if (global_dev->udev == NULL)
   {
     printk(KERN_INFO KBUILD_MODNAME "global uDev == NULL!\n");
     goto exit;
   }
  //printk("---NUM OF ENDPOINTS: %d ---\n",interface->cur_altsetting->desc.bNumEndpoints);
  
  dev = usb_get_intfdata(interface);
  if (dev == NULL)
  {
    printk(KERN_INFO KBUILD_MODNAME "dev == NULL");
    retval = -ENODEV;
    goto exit;
  }
  
  // Increment our usage count for the device. 
  //++dev->open_count;
  //if (dev->open_count > 1)
  //  {
  //    printk(KERN_INFO KBUILD_MODNAME"open_count = %d", dev->open_count);
  //  }
  
  if(!dev->udev)
    {
      printk("NO UDEVS!!!!! --------\n");
      goto exit;
    }
  if(!usb_rcvintpipe(dev->udev, dev->int_in_endpoint->bEndpointAddress))
    {
      printk("NO RCV PIPE!!!!! --------\n");
      goto exit;
    }
  else
    {
      printk("endpoint address = %x --------\n",dev->int_in_endpoint->bEndpointAddress );
    }
  if(!dev->int_in_buffer)
    {
      printk("NO INT IN BUFFER!!!!! --------\n");
      goto exit;
    }
  if(!le16_to_cpu(dev->int_in_endpoint->wMaxPacketSize))
    {
      printk("NO MAX PACKET SIZE!!!!! --------\n");
      goto exit;
    }
  
  if(!dev->int_in_endpoint->bInterval)
    {
      printk("NO BINTERVAL!!!!! --------\n");
      goto exit;
    }
  else
    {
      printk("interrupt enpoint->bInterval = %d\n", dev->int_in_endpoint->bInterval);
    }
  //insitialise the control URB  -according to cdc-acm
  //dev->int_in_urb->dev = dev->udev;
  printk("------GOT TO HERE-------\n");
  //
  //  usb_fill_int_urb(dev->int_in_urb, dev->udev,
  //		     usb_rcvintpipe(dev->udev, dev->int_in_endpoint->bEndpointAddress),
  //		     dev->int_in_buffer, le16_to_cpu(dev->int_in_endpoint->wMaxPacketSize),
  //		     pio_int_in_callback, dev, dev->int_in_endpoint->bInterval);
  //

  //  strcpy(dev->bulk_out_buffer, (char*)buffer);

  for (i = 0 ; i < 7 ; i++)
    {
      dev->bulk_in_buffer[i] = buffer[i];
      printk("%c\n", dev->bulk_in_buffer[i]);
    }

  usb_fill_bulk_urb(dev->bulk_in_urb, dev->udev, 
		    usb_sndbulkpipe(dev->udev, dev->bulk_in_endpoint->bEndpointAddress), 
		    dev->bulk_in_buffer, le16_to_cpu(dev->bulk_in_endpoint->wMaxPacketSize),
		    pio_int_in_callback, dev);
  dev->bulk_in_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
  printk("------GOT PAST THERE-------\n");
  printk("buffer string %s\n", global_dev->bulk_in_buffer); 
  
  dev->int_in_running = 1;
  //mb();  //--> not sure what this is??
  printk(KERN_INFO KBUILD_MODNAME " Submitting init urb\n");
  if (usb_submit_urb(dev->bulk_in_urb,GFP_KERNEL))
    {
      printk(KERN_INFO KBUILD_MODNAME "failed to submit init urb\n");
      dev->int_in_running = 0;
      --dev->open_count;
      goto unlock_exit;
    }

  // Save our object in the file's private structure.
  file->private_data = global_dev;
  
  //usb_autopm_put_interface(dev->control_interface);
  printk(KERN_INFO KBUILD_MODNAME"------- I'm open minor number %d-------\n", subminor);

unlock_exit:
  	//up(&dev->sem);
exit:
	//mutex_unlock(&disconnect_mutex);
	return retval;
}

static int pio_release (struct inode *inode, struct file *file)
{
  struct usb_pio *dev = NULL;
  int retval = 0;
 // 
 // dev = file->private_data;
 // 
 // if (! dev)
 //   {
 //     printk(KERN_INFO KBUILD_MODNAME"dev is NULL");
 //     retval =  -ENODEV;
 //     goto exit;
 //   }
 // 
 // if (dev->open_count <= 0)
 //   {
 //     printk(KERN_INFO KBUILD_MODNAME"device not opened");
 //     retval = -ENODEV;
 //     goto unlock_exit;
 //   }
 // 
 // if (! dev->udev)
 //   {
 //     printk(KERN_INFO KBUILD_MODNAME"device unplugged before the file was released");
 //     up (&dev->sem);	/* Unlock here as ml_delete frees dev. */
 //     pio_delete(dev);
 //     goto exit;
 //   }
 // 
 // if (dev->open_count > 1)
 //   {
 //     printk(KERN_INFO KBUILD_MODNAME"open_count = %d", dev->open_count);
 //   }
 // 
 // pio_abort_transfers(dev);
 // --dev->open_count;
 //
 //unlock_exit:
 // //up(&dev->sem);
 // 
 //exit:
  return retval;
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

static struct urb* initialise_urb(int* urb_err)
{
  struct urb* init_urb = usb_alloc_urb(0,GFP_KERNEL);
  if (! init_urb)
  {
    urb_err = urb_err + 1;
  }    
  return init_urb;
  
  
}
/*
 *
 */
static int pio_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  struct usb_device *udev = interface_to_usbdev(interface);
  struct usb_pio *dev = NULL;
  struct usb_host_interface *iface_desc;
  int i;
  int retval = -ENODEV;
  int int_flag = 0, bulk_flag_in = 0, bulk_flag_out = 0, buf_err = 0, urb_err = 0;
  u8 call_management_function = 3;
  int call_interface_num = 14;
  struct usb_interface *control_interface;
  struct usb_interface *data_interface;

  //3 14 642 mI 0 sI 1
  //collate interfaces 
   iface_desc = interface->cur_altsetting;
   control_interface = usb_ifnum_to_if(udev, 0);
   data_interface = usb_ifnum_to_if(udev, 1);
  
  if (! udev)
    {
      //DBG_ERR("udev is NULL");
      printk(KERN_INFO KBUILD_MODNAME": udev is null");
      goto exit;
    }

  dev = kzalloc(sizeof(struct usb_pio), GFP_KERNEL);
  global_dev = kzalloc(sizeof(struct usb_pio), GFP_KERNEL);
  if (! dev)
  {
    //DBG_ERR("cannot allocate memory for struct usb_pio");
    printk(KERN_INFO KBUILD_MODNAME": Cannot allocate memory for struct usb_pio");
    retval = -ENOMEM;
    goto exit;
  }

  //dev->command = PIO_STOP;

  sema_init(&dev->sem,1);
  spin_lock_init(&dev->cmd_spinlock);
  
  printk(KERN_INFO KBUILD_MODNAME ": %d endpoints found\n", iface_desc->desc.bNumEndpoints);
  //add some checking here so that it doesn't crash 
  global_dev->int_in_endpoint = &control_interface->cur_altsetting->endpoint[0];
 
  global_dev->int_in_urb = initialise_urb(&urb_err);
  global_dev->bulk_in_endpoint = &data_interface->cur_altsetting->endpoint[1];
 
  global_dev->bulk_in_urb = initialise_urb(&urb_err);
  global_dev->bulk_out_endpoint = &data_interface->cur_altsetting->endpoint[2];
  global_dev->bulk_out_urb = initialise_urb(&urb_err);
  
  global_dev->control_interface = control_interface;
  global_dev->data_interface = data_interface;
  global_dev->udev = udev;

  global_dev->int_in_buffer = usb_alloc_coherent(global_dev->udev, sizeof(char[8]),
						 GFP_KERNEL, &global_dev->int_in_urb->transfer_dma);
  global_dev->bulk_in_buffer = usb_alloc_coherent(global_dev->udev,sizeof(char[32]), 
						  GFP_KERNEL, &global_dev->bulk_in_urb->transfer_dma);
  global_dev->bulk_out_buffer = usb_alloc_coherent(global_dev->udev,sizeof(char[32]),
						   GFP_KERNEL, &global_dev->bulk_out_urb->transfer_dma);   
  if ((! global_dev->int_in_endpoint) && (int_flag))
    {
      printk(KERN_INFO KBUILD_MODNAME": could not find interupt in endpoint");
      goto error;
    }
  if ((! global_dev->bulk_in_endpoint)&& (bulk_flag_in))
    {
      printk(KERN_INFO KBUILD_MODNAME": could not find bulk in endpoint");
      goto error;
    }
  if ((! global_dev->bulk_out_endpoint)&& (bulk_flag_out))
    {
      printk(KERN_INFO KBUILD_MODNAME": could not find bulk out endpoint");
      goto error;
    }
  //loading up usb_pio


  /* --------------------------------------------------------------------------------------------------
   *
   * 	THIS IS WHERE IT BREAKS ON PROBE!!!
   *
   */
  usb_fill_int_urb(global_dev->int_in_urb, udev,
		   usb_rcvintpipe(udev, global_dev->int_in_endpoint->bEndpointAddress),
		   global_dev->int_in_buffer, le16_to_cpu(global_dev->int_in_endpoint->wMaxPacketSize), 
		   pio_int_in_callback, global_dev,
		   /* works around buggy devices */
		   global_dev->int_in_endpoint->bInterval ? global_dev->int_in_endpoint->bInterval : 0xff);

  	//printk("pio%d: USB PIO device\n", minor);

  usb_set_intfdata(interface, global_dev);
  //i = device_create_file(&interface->dev, &dev_attr_bmCapabilities);
  printk("=====country code = %d =====\n",i);
  usb_driver_claim_interface(&usb_pio_driver, data_interface, dev);
  usb_set_intfdata(data_interface, global_dev);
  //global_dev = dev;
  //usb_get_intfdata(control_interface);
  /* We can register the device now, as it is ready */
  retval = usb_register_dev(interface, &pio_class);  //cdc-acm driver dones't think we need this

error:
  pio_delete(dev);
  return retval;

exit:
  return retval;
}

static void pio_disconnect(struct usb_interface *interface)
{
	struct usb_pio *dev;
        struct usb_device* usb_dev = interface_to_usbdev(interface);
	int minor;

	printk("-------DC-------\n");

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	if(!dev)
    {
		printk("Dev is null\n\n");
		return;
    }

	mutex_lock(&disconnect_mutex);	/* Not interruptible */

	/* Give back our minor. */
	minor = dev->minor;

	usb_deregister_dev(interface, &pio_class);

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
