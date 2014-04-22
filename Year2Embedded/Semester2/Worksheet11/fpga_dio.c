#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h> /*Needed for KERN_ALERT*/
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/arch/io.h>
#include <asm/arch/hardware.h>
#include <asm/uaccess.h>

/*ETHERNET LIGHTSSSSSS*/

#define DRIVER_AUTHOR "craig duffy craig.duffy@uwe.ac.uk"
#define DRIVER_DESC   "FPGA DIO driver"
#define LED_ADDRESS        0xf2400680
#define RGGG  0xf000

static int fpga_dio_init(void)
{
	static int fpga_j;
	static short unsigned pattern;

	printk(KERN_ALERT "fpga dio loaded\n");

	pattern=RGGG;
	pattern = pattern >> 4;

	for ( fpga_j=0; fpga_j != 16 ; fpga_j++)
	{
		printk("pattern %x\n",pattern);
		udelay(40000);
		writew(pattern,LED_ADDRESS);
		pattern = pattern >> 8;
		pattern--;
		pattern = pattern << 8;
	}

	return 0;
}


static void fpga_dio_exit(void)
{
	printk(KERN_ALERT "fpga dio unloaded\n");
}


module_init(fpga_dio_init);
module_exit(fpga_dio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("fpga_dio");
