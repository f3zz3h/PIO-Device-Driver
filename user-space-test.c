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

/* *****************************************************************
 *
 *
 * *****************************************************************/
int main(void)
{
	FILE* driverFp;
	int ret = 1;
	driverFp = fopen("/dev/pio0", "r+");
	printf("OPENING DRIVER!!\n");
	getchar();
	if (driverFp)
	{
		printf("WRITING\n");
		ret = fwrite("@00P2FF\r", 8, sizeof("@00P2FF\r"), driverFp);
		printf("write ret val = %d error num = %d err: %s\n", ret, errno,
				strerror(errno));
		getchar();
		// ret = ioctl(driverFp, ret, ret);
		//printf("ioctl ret val = %d error num = %d err: %s\n", ret, errno, strerror(errno));
	}
	
		fclose(driverFp);
	printf("Close DRIVERS!!\n");
}
