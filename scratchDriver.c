/******************************************************************
 * usb driver written from scratch following ML skeleton code
 *
 * *****************************************************************/
#include "scratchDriver.h"

static struct file_operations pio_fops =
{
		.owner = THIS_MODULE,
		.write = pio_write,
		.read = pio_read,
		.open = pio_open,
		.release = 	pio_release,
		//.unlocked_ioctl = pio_ioctl,
};

static struct usb_class_driver pio_class =
{
		.name = "pio%d",
		.fops = &pio_fops,
		.minor_base = PIO_MINOR_BASE,
};

static struct usb_driver usb_pio_driver =
{
		.name = "usb_pio",
		.id_table = pio_id_table,
		.probe = pio_probe,
		.disconnect = pio_disconnect,
};

/* *****************************************************************
 *
 *
 * *****************************************************************/
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
/* *****************************************************************
 *
 *
 * *****************************************************************/
static void pio_delete(struct usb_pio *dev)
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
/* *****************************************************************
 *
 *
 * *****************************************************************/
static void pio_int_in_callback(struct urb *urb)
{
	printk("----Completion handler----contest=%d\n", urb->status);

	if (urb->status && !(urb->status == -ENOENT || urb->status == -ECONNRESET
			|| urb->status == -ESHUTDOWN))
	{
		printk("Non zero status recieved\n status ");
	}
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			urb->transfer_buffer, urb->transfer_dma);
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static long pio_ioctl(struct file *file, unsigned int cmd, unsigned long int arg)
{

	switch (cmd)
	{
	case 1:
		printk("----- IM IOCTL %d here my message %s------\n", cmd,
				(char *) arg);
		break;
	default:
		printk("--OMG! CMD:%d - arg:%d --\n", cmd, (unsigned int)arg);
	}

	return 0;
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static int pio_open(struct inode *inode, struct file *file)
{
	struct usb_pio *dev = NULL;
	struct usb_interface *interface;
	int subminor, len;
	int retval = 0;
	char buffer[8] = { '@', '0', '0', 'D', '0', '0', '0', '\r' };
	char buffer2[8] ={ '@', '0', '0', 'D', '1', 'F', 'F', '\r' };
	char buffer3[8] = { '@', '0', '0', 'D', '2', '0', '0', '\r' };
	char buffer4[8] = { '@', '0', '0', 'P', '0', '0', '1', '\r' };
	char buffer5[8] = { '@', '0', '0', 'P', '2', '3', 'F', '\r' };
	int i;
	printk("---Entering Open---");
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
	dev = usb_get_intfdata(interface);
	if (dev == NULL)
	{
		printk(KERN_INFO KBUILD_MODNAME "dev == NULL");
		retval = -ENODEV;
		goto exit;
	}
	if (dev->data_dev == NULL)
	  {
	    	printk(KERN_INFO KBUILD_MODNAME "data_dev == NULL");
		retval = -ENODEV;
		goto exit;
	  }
	for (i = 0; i < 7; i++)
	{
		dev->bulk_out_buffer[i] = buffer[i];
		//printk("%c\n", dev->bulk_in_buffer[i]);
	}
	
	len = strlen(dev->bulk_out_buffer);
	printk ("--- buffer length = %d\n", len); 
	usb_bulk_msg(dev->udev, 
		     usb_sndbulkpipe(dev->udev, dev->bulk_out_endpoint->bEndpointAddress),
		     buffer,
		     8,
		     &len,
		     10000);

	usb_bulk_msg(dev->udev,
		     usb_sndbulkpipe(dev->udev, dev->bulk_out_endpoint->bEndpointAddress),
		     buffer2,
		     min(sizeof(dev->bulk_out_buffer), 8),
		     &len,
		     10000);

	usb_bulk_msg(dev->udev,
		     usb_sndbulkpipe(dev->udev, dev->bulk_out_endpoint->bEndpointAddress),
		     buffer3,
		     min(sizeof(dev->bulk_out_buffer), 8),
		     &len,
		     10000);

	usb_bulk_msg(dev->udev,
		     usb_sndbulkpipe(dev->udev, dev->bulk_out_endpoint->bEndpointAddress),
		     buffer4,
		     min(sizeof(dev->bulk_out_buffer), 8),
		     &len,
		     10000);
	/*
	usb_bulk_msg(dev->udev,
		     usb_sndbulkpipe(dev->udev, dev->bulk_out_endpoint->bEndpointAddress),
		     buffer5,
		     min(sizeof(dev->bulk_out_buffer), 8),
		     &len,
		     10000);
	*/
	
	printk("buffer string %s\n", dev->bulk_out_buffer);

	dev->int_in_running = 1;

	printk(KERN_INFO KBUILD_MODNAME " Submitting init urb\n");
	/*	
	if (retval = usb_submit_urb(dev->bulk_out_urb, GFP_KERNEL))
	{
	  printk(KERN_INFO KBUILD_MODNAME "failed to submit init urb err no %x\n",dev->bulk_out_endpoint->bEndpointAddress );
		goto unlock_exit;
	}
	*/
	// Save our object in the file's private structure.
	file->private_data = dev;

	//usb_autopm_put_interface(dev->control_interface);
	printk(KERN_INFO KBUILD_MODNAME"------- I'm open minor number %d-------\n", subminor);

	unlock_exit:
	//up(&dev->sem);
	exit:
	//mutex_unlock(&disconnect_mutex);
	return retval;
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static int pio_release(struct inode *inode, struct file *file)
{
	struct usb_pio *dev = NULL;
	int retval = 0;

	printk("----------RELEASEING\n");

	dev = file->private_data;

	if (!dev)
	{
		printk(KERN_INFO KBUILD_MODNAME"dev is NULL");
		retval = -ENODEV;
		goto exit;
	}
	if (!dev->udev)
	{
		printk(KERN_INFO KBUILD_MODNAME"device unplugged before the file was released");
		//     up (&dev->sem);	/* Unlock here as ml_delete frees dev. */
		pio_delete(dev);
		goto exit;
	}
	//
	//pio_abort_transfers(dev);

	exit: return retval;
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static ssize_t pio_write (struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	struct usb_pio *dev;
	int retval = 0, len;
	u8 buf[PIO_CTRL_BUFFER_SIZE];
	/* Not sure if command should have some better intialization!? */
	__u8 cmd = 0;

	printk("-----------WRITING: %s\n", user_buf);

	dev = file->private_data;

	/* Verify that the device wasn't unplugged. */
	if (! dev->udev)
	{
		retval = -ENODEV;
		printk("No device or device unplugged (%d)", retval);
		goto exit;
	}

	/* Verify that we actually have some data to write. */
	if (count == 0)
	{
		printk("No data to write\n");
		goto exit;
	}

	if (copy_from_user(dev->bulk_out_buffer, user_buf, 8))
	{
		retval = -EFAULT;
		goto exit;
	}
	/* The interrupt-in-endpoint handler also modifies dev->command. */
	//	spin_lock(&dev->cmd_spinlock);
	//dev->command = cmd;
	//spin_unlock(&dev->cmd_spinlock);	
	len = strlen(dev->bulk_out_buffer);
	retval = usb_bulk_msg(dev->data_dev, 
		     usb_sndbulkpipe(dev->data_dev, dev->bulk_out_endpoint->bEndpointAddress),
		     dev->bulk_out_buffer,
		     8,
		     &len,
		     10000);

	/* FixMe: Pretty sure this is going to the wrong endpoint
	 * int usb_control_msg (struct usb_device * dev, unsigned int pipe,
	 * __u8 request, __u8 requesttype, __u16 value, __u16 index, void * data,
	 * __u16 size, int timeout); */
	/*	retval = usb_control_msg(dev->udev,
			usb_sndctrlpipe(dev->udev, 0),
			PIO_CTRL_REQUEST,
			PIO_CTRL_REQEUST_TYPE,
			PIO_CTRL_VALUE,
			PIO_CTRL_INDEX,
			&buf,
			sizeof(buf),
			10000);
	*/
	if (retval < 0)
	{
		printk("usb_control_msg failed (%d)", retval);
		goto exit;
	}
	retval = count;

	exit:
	return retval;
}
static ssize_t pio_read (struct file *file, const char __user *user_buf, size_t count, loff_t *f_pos)
{
  struct usb_pio *dev;
  int retval = 0, len, i;
  
  dev = file->private_data;
  
  retval = usb_bulk_msg(dev->udev,
			usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpoint->bEndpointAddress),
			dev->bulk_in_buffer,
			min(sizeof(dev->bulk_in_buffer), count),
			&len, 
			10000);

  
/* if the read was successful, copy the data to userspace */
if (retval == 0) {
  if (copy_to_user(user_buf, dev->bulk_in_buffer, len))
    retval = -EFAULT;
  else
    retval = count;
}

return retval;
/*if (! dev->udev)
	{
		retval = -ENODEV;
		printk("No device or device unplugged (%d)", retval);
		return retval;
	}
  if (file->f_flags & O_NONBLOCK)
  {
      return -EAGAIN;
  }

  spin_lock_irq(&dev->bulk_in_lock);
  len = dev->bulk_in_ptr - dev->bulk_in_buffer;
  if(len > count)
    {
      len = count;
    }

  if (dev->bulk_in_buffer + len == dev->bulk_in_ptr)
    {
      dev->bulk_in_ptr = dev->bulk_in_buffer;
    }
  else
    {
      for (i = 0; i < dev->bulk_in_ptr - dev->bulk_in_buffer - len; i++)
	{
	  dev->bulk_in_buffer[i] = dev->bulk_in_buffer[i + len];
	}
      dev->bulk_in_ptr = dev->bulk_in_buffer + len;
    }
  dev->read_ready = 0;
  spin_unlock_irq(&dev->bulk_in_lock);
  return len;
*/	
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static struct urb* initialise_urb(int* urb_err)
{
	struct urb* init_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!init_urb)
	{
		urb_err = urb_err + 1;
	}
	return init_urb;

}


/* *****************************************************************
 *
 *
 * *****************************************************************/
static int pio_probe(struct usb_interface *interface,
		const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_device *ddev;
	struct usb_pio *dev = NULL;
	struct usb_host_interface *iface_desc;	
	int retval = -ENODEV;
	int int_flag = 0, bulk_flag_in = 0, bulk_flag_out = 0, urb_err = 0, i;
	struct usb_interface *control_interface;
	struct usb_interface *data_interface;
	struct usb_endpoint_descriptor *tempEndpoint;


	//3 14 642 mI 0 sI 1
	//collate interfaces
	iface_desc = interface->cur_altsetting;
	control_interface = usb_ifnum_to_if(udev, 0);
	data_interface = usb_ifnum_to_if(udev, 1);
	ddev = interface_to_usbdev(data_interface);
	if (!udev)
	{
		//DBG_ERR("udev is NULL");
		printk(KERN_INFO KBUILD_MODNAME": udev is null");
		goto exit;
	}

	dev = kzalloc(sizeof(struct usb_pio), GFP_KERNEL);
	global_dev = kzalloc(sizeof(struct usb_pio), GFP_KERNEL);
	if (!dev)
	{
		printk("cannot allocate memory for struct usb_pio");
		printk(KERN_INFO KBUILD_MODNAME": Cannot allocate memory for struct usb_pio");
		retval = -ENOMEM;
		goto exit;
	}

	//	sema_init(&global_dev->sem, 1);
	//spin_lock_init(&global_dev->cmd_spinlock);

	printk(KERN_INFO KBUILD_MODNAME ": %d endpoints found\n", iface_desc->desc.bNumEndpoints);

	//get endpoints
	iface_desc = data_interface->cur_altsetting;
	printk(KERN_INFO KBUILD_MODNAME ": %d endpoints foundin data interface\n", iface_desc->desc.bNumEndpoints);
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
	  {
	    tempEndpoint = &iface_desc->endpoint[i].desc;
	    if (tempEndpoint->bEndpointAddress == BULK_ENDPOINT_ADDRESS_IN)
	      {
		printk(KERN_INFO KBUILD_MODNAME ": endpoint[%d] is bulk_in\n", i);
		global_dev->bulk_in_endpoint =  &iface_desc->endpoint[i].desc;
	      }
	    else if (tempEndpoint->bEndpointAddress == BULK_ENDPOINT_ADDRESS_OUT)
	      {
		printk(KERN_INFO KBUILD_MODNAME ": endpoint[%d] is bulk_out\n", i);
		global_dev->bulk_out_endpoint =  &iface_desc->endpoint[i].desc;
	      }
	  }
	
	//add some checking here so that it doesn't crash
	/* ToDo: Are these assignments correct matty?? */

	global_dev->bulk_in_urb = initialise_urb(&urb_err);      
	global_dev->bulk_out_urb = initialise_urb(&urb_err);

	global_dev->control_interface = control_interface;
	global_dev->data_interface = data_interface;
	global_dev->udev = udev;
	global_dev->data_dev = ddev;

	//	global_dev->int_in_buffer = usb_alloc_coherent(global_dev->udev,
	//			sizeof(char[8]), GFP_KERNEL, &global_dev->int_in_urb->transfer_dma);
	global_dev->bulk_in_buffer = usb_alloc_coherent(global_dev->udev,
			sizeof(char[32]), GFP_KERNEL,
			&global_dev->bulk_in_urb->transfer_dma);
	global_dev->bulk_out_buffer = usb_alloc_coherent(global_dev->udev,
			sizeof(char[32]), GFP_KERNEL,
			&global_dev->bulk_out_urb->transfer_dma);

	global_dev->open_count = 0;
	//removed int_in_endpoint stuff
	if ((!global_dev->bulk_in_endpoint) && (bulk_flag_in))
	{
		printk(KERN_INFO KBUILD_MODNAME": could not find bulk in endpoint");
		goto error;
	}
	if ((!global_dev->bulk_out_endpoint) && (bulk_flag_out))
	{
		printk(KERN_INFO KBUILD_MODNAME": could not find bulk out endpoint");
		goto error;
	}
	
	usb_set_intfdata(interface, global_dev);

	usb_driver_claim_interface(&usb_pio_driver, data_interface, dev);
	usb_set_intfdata(data_interface, global_dev);

	//We can register the device now, as it is ready 
	retval = usb_register_dev(interface, &pio_class); //cdc-acm driver dones't think we need this

	exit:
		return retval;

	error: pio_delete(dev);
		return retval;

}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static void pio_disconnect(struct usb_interface *interface)
{
	struct usb_pio *dev;
	int minor;

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	if (!dev)
	{
		printk("Dev is null\n");
		return;
	}

	//	mutex_lock(&disconnect_mutex); /* Not interruptible */
	//	down(&dev->sem);

	/* Give back our minor. */
	minor = dev->minor;

	usb_deregister_dev(interface, &pio_class);

	/*ToDo: get this working */
	/*
	 if (!dev->open_count)
	{
		up(&dev->sem);
		pio_delete(dev);
	}
	else
	{
		dev->udev = NULL;
		up(&dev->sem);
	}
	*/

	//	mutex_unlock(&disconnect_mutex);

	printk("USB-PIO /dev/pio%d now disconnected\n", minor - PIO_MINOR_BASE);
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static int __init usb_pio_init(void)
{
	int result;
	printk(KERN_INFO KBUILD_MODNAME": Register device driver!\n");
	result = usb_register(&usb_pio_driver);

	if (result)
	{
		printk(KERN_INFO KBUILD_MODNAME": Driver not Registered, Retval=%d\n",result);
	}
	else
	{
		printk(KERN_INFO KBUILD_MODNAME": Driver Registered\n");
	}
	return 0;
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static void __exit usb_pio_exit(void)
{
	printk(KERN_INFO KBUILD_MODNAME": Exited\n");
	usb_deregister(&usb_pio_driver);
}

module_init( usb_pio_init);
module_exit( usb_pio_exit);

MODULE_AUTHOR("Matt Hall, Luke Hart, Joe Ellis, Jake Baker, Seb Beaven, Adam Tyndale");
MODULE_LICENSE("GPL");
