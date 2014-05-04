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
int writeToDriver(int fd, char* buffer);

int main(void)
{
	int driverFp;
	int ret = 1;
	char* message = "hello";
	char buffer1[8] = { '@', '0', '0', 'D', '0', '0', '0', '\r' };
	char buffer2[8] = { '@', '0', '0', 'D', '1', 'F', 'F', '\r' };
	char buffer3[8] = { '@', '0', '0', 'D', '2', '0', '0', '\r' };
	char buffer4[8] = { '@', '0', '0', 'P', '0', '0', '1', '\r' };
	char buffer5[8] = { '@', '0', '0', 'P', '2', '3', 'F', '\r' };
	driverFp = open("/dev/pio0", O_RDWR);
	printf("OPENING DRIVER!!\n");
	
	getchar();
	if (driverFp)
	{
	  printf("WRITING\n");
	 
	  writeToDriver(driverFp, buffer1);
	  writeToDriver(driverFp, buffer2);
	  writeToDriver(driverFp, buffer3);
	  writeToDriver(driverFp, buffer4);
	  getchar();
	  writeToDriver(driverFp, buffer5);
	  getchar();
	  writeToDriver(driverFp, "@00P00F\r");
	  getchar();
	  writeToDriver(driverFp, "@00P23B\r");
	  getchar();
	  ret = ioctl(driverFp, 1,(unsigned long) message);
	  printf("ioctl ret val = %d error num = %d err: %s\n", ret, errno, strerror(errno));
	  getchar();
	}
	
	close(driverFp);
	printf("Close DRIVERS!!\n");
}

int writeToDriver(int fd, char* buffer)
{
  int ret;  
  ret = write (fd,buffer, sizeof(buffer));
  printf("write ret val = %d error num = %d err: %s\n", ret, errno,
	 strerror(errno));
}
