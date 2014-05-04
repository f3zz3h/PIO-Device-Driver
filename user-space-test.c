/* *****************************************************************
 * usb driver written from scratch following ML skeleton code
 * C program for opening and writing/reading to the driver
 * *****************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>

/* *****************************************************************
 *
 *
 * *****************************************************************/
int main(void)
{
	FILE* driverFp;
	int ret = 1;
	char* message = "hello";
	driverFp = open("/dev/pio0", O_RDWR);
	printf("OPENING DRIVER!!\n");
	
	getchar();
	if (driverFp)
	{
	  printf("WRITING\n");
	  /*
	  ret = write("@00P2FF\r", 8, sizeof("@00P2FF\r"), driverFp);
	  printf("write ret val = %d error num = %d err: %s\n", ret, errno,
		 strerror(errno));
	  getchar();
	  
	  ret = write("@00P2FC\r", 8, sizeof("@00P2FC\r"), driverFp);
	  printf("write ret val = %d error num = %d err: %s\n", ret, errno,
		 strerror(errno));
	  getchar();
	  */
	  ret = ioctl((int)driverFp, ret,(unsigned long) message );
	  printf("ioctl ret val = %d error num = %d err: %s\n", ret, errno, strerror(errno));
	  getchar();
	}
	
	close(driverFp);
	printf("Close DRIVERS!!\n");
}
