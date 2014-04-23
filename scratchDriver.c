/* *****************************************************************
 * usb driver written from scratch following ML skeleton code
 *
 * *****************************************************************/
#include "scratchDriver.h"

static struct file_operations pio_fops =
{
		.owner = THIS_MODULE,
		.write = pio_write,
		.open = pio_open,
		.release = pio_release,
		.unlocked_ioctl = pio_ioctl,
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
	struct usb_pio *dev = urb->context;

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
int pio_ioctl(struct file *file, unsigned int cmd, unsigned long int arg)
{
	switch (cmd)
	{
	case 1:
		printk("\n----- IM IOCTL %d here my message %s------\n", cmd,
				(char *) arg);
		break;
	default:
		printk("\n--------ERMERGERD!!-------\n");
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
	int subminor;
	int retval = 0;
	char buffer[32] =
	{ '@', '0', '0', 'P', '2', '3', 'F', '\r' };
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
	dev = usb_get_intfdata(interface);
	if (dev == NULL)
	{
		printk(KERN_INFO KBUILD_MODNAME "dev == NULL");
		retval = -ENODEV;
		goto exit;
	}

	for (i = 0; i < 7; i++)
	{
		dev->bulk_in_buffer[i] = buffer[i];
		printk("%c\n", dev->bulk_in_buffer[i]);
	}

	usb_fill_bulk_urb(
			dev->bulk_in_urb,
			dev->udev,
			usb_sndbulkpipe(dev->udev, dev->bulk_in_endpoint->bEndpointAddress),
			dev->bulk_in_buffer,
			le16_to_cpu(dev->bulk_in_endpoint->wMaxPacketSize),
			pio_int_in_callback, dev);
	dev->bulk_in_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	printk("buffer string %s\n", global_dev->bulk_in_buffer);

	dev->int_in_running = 1;

	printk(KERN_INFO KBUILD_MODNAME " Submitting init urb\n");

	if (usb_submit_urb(dev->bulk_in_urb, GFP_KERNEL))
	{
		printk(KERN_INFO KBUILD_MODNAME "failed to submit init urb\n");
		dev->int_in_running = 0;
		--dev->open_count;
		goto unlock_exit;
	}

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
	pio_abort_transfers(dev);

	exit: return retval;
}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static ssize_t pio_write (struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	printk("-----------WRITING: %s\n", user_buf);
	return 0;
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
static int pio_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_pio *dev = NULL;
	struct usb_host_interface *iface_desc;
	int retval = -ENODEV;
	int int_flag = 0, bulk_flag_in = 0, bulk_flag_out = 0, urb_err = 0;
	//unused?? int buf_err = 0;
	//unused?? u8 call_management_function = 3;
	//unused?? int call_interface_num = 14;
	struct usb_interface *control_interface;
	struct usb_interface *data_interface;

	//3 14 642 mI 0 sI 1
	//collate interfaces
	iface_desc = interface->cur_altsetting;
	control_interface = usb_ifnum_to_if(udev, 0);
	data_interface = usb_ifnum_to_if(udev, 1);

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
		//DBG_ERR("cannot allocate memory for struct usb_pio");
		printk(KERN_INFO KBUILD_MODNAME": Cannot allocate memory for struct usb_pio");
		retval = -ENOMEM;
		goto exit;
	}

	//dev->command = PIO_STOP;

	sema_init(&dev->sem, 1);
	spin_lock_init(&dev->cmd_spinlock);

	printk(KERN_INFO KBUILD_MODNAME ": %d endpoints found\n", iface_desc->desc.bNumEndpoints);

	//add some checking here so that it doesn't crash
	global_dev->int_in_endpoint
			= &control_interface->cur_altsetting->endpoint[0];

	global_dev->int_in_urb = initialise_urb(&urb_err);
	global_dev->bulk_in_endpoint = &data_interface->cur_altsetting->endpoint[1];

	global_dev->bulk_in_urb = initialise_urb(&urb_err);
	global_dev->bulk_out_endpoint
			= &data_interface->cur_altsetting->endpoint[2];
	global_dev->bulk_out_urb = initialise_urb(&urb_err);

	global_dev->control_interface = control_interface;
	global_dev->data_interface = data_interface;
	global_dev->udev = udev;

	global_dev->int_in_buffer = usb_alloc_coherent(global_dev->udev,
			sizeof(char[8]), GFP_KERNEL, &global_dev->int_in_urb->transfer_dma);
	global_dev->bulk_in_buffer = usb_alloc_coherent(global_dev->udev,
			sizeof(char[32]), GFP_KERNEL,
			&global_dev->bulk_in_urb->transfer_dma);
	global_dev->bulk_out_buffer = usb_alloc_coherent(global_dev->udev,
			sizeof(char[32]), GFP_KERNEL,
			&global_dev->bulk_out_urb->transfer_dma);
	if ((!global_dev->int_in_endpoint) && (int_flag))
	{
		printk(KERN_INFO KBUILD_MODNAME": could not find interupt in endpoint");
		goto error;
	}
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

	//loading up usb_pio
	usb_fill_int_urb(
			global_dev->int_in_urb,
			udev,
			usb_rcvintpipe(udev, global_dev->int_in_endpoint->bEndpointAddress),
			global_dev->int_in_buffer,
			le16_to_cpu(global_dev->int_in_endpoint->wMaxPacketSize),
			pio_int_in_callback,
			global_dev,
			/* works around buggy devices */
			global_dev->int_in_endpoint->bInterval ? global_dev->int_in_endpoint->bInterval
					: 0xff);

	usb_set_intfdata(interface, global_dev);

	usb_driver_claim_interface(&usb_pio_driver, data_interface, dev);
	usb_set_intfdata(data_interface, global_dev);

	/* We can register the device now, as it is ready */
	retval = usb_register_dev(interface, &pio_class); //cdc-acm driver dones't think we need this

	exit:
		return retval;
	error:
		pio_delete(dev);
		return retval;

}
/* *****************************************************************
 *
 *
 * *****************************************************************/
static void pio_disconnect(struct usb_interface *interface)
{
	struct usb_pio *dev;
	//unused?? struct usb_device* usb_dev = interface_to_usbdev(interface);
	int minor;

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	if (!dev)
	{
		printk("Dev is null\n");
		return;
	}

	mutex_lock(&disconnect_mutex); /* Not interruptible */

	/* Give back our minor. */
	minor = dev->minor;

	usb_deregister_dev(interface, &pio_class);

	mutex_unlock(&disconnect_mutex);

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

module_init(usb_pio_init);
module_exit(usb_pio_exit);

MODULE_AUTHOR("Matt Hall, Luke Hart, Joe Ellis, Jake Baker, Seb Beaven, Adam Tyndale");
MODULE_LICENSE("GPL");
