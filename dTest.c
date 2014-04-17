//C program for opening and writing/reading to the driver

#include <stdio.h>

int main (void)
{
  FILE *driverFp;
  driverFp = fopen("/dev/pio0", "r");
  printf("OPENING DRIVER!!\n");
  //fclose(driverFp);
  printf("Close DRIVERS!!\n");
}
