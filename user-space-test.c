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
int main (void)
{
  FILE* driverFp;
  int ret=1;
  driverFp = fopen("/dev/pio0","r+");
  printf("OPENING DRIVER!!\n");  
  if (driverFp)
    {
      printf("WRITING\n");
      ret =fwrite("a",1, sizeof("a"), driverFp);
      printf("write ret val = %d error num = %d err: %s\n", ret, errno, strerror(errno));
      // ret = ioctl(driverFp, ret, ret);
      //printf("ioctl ret val = %d error num = %d err: %s\n", ret, errno, strerror(errno));
    }
  //  fclose(driverFp);
  //printf("Close DRIVERS!!\n");
}
