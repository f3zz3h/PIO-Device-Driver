/* *****************************************************************
 * usb driver written from scratch following ML skeleton code
 * C program for opening and writing/reading to the driver
 * *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <math.h>
#include "SDU.h"

/* *****************************************************************
 *
 *
 * *****************************************************************/
int writeToDriver(int fd, char* buffer);
char* setDisplayValue(char charSelect);
void getInput(char* input[4]);
void getInputK(int fd, char* input);
void numberLoop(int driverFp);
char readKeys(int driverFp);
void numberLoopFromKeypad(int driverFp, char* input[4]);


int main(void)
{
	int driverFp;

	driverFp = open("/dev/pio1", O_RDWR); //change to correct pio

	if (driverFp)
	{
		numberLoop(driverFp);
		//readKeys(driverFp);
	}
	close(driverFp);
	printf("Exit!\n");
}
void numberLoop(int driverFp)
{
	char buffer1[8] = { '@', '0', '0', 'D', '0', '0', '0', '\r' };
	char buffer2[8] = { '@', '0', '0', 'D', '1', 'F', 'F', '\r' };
	char buffer3[8] = { '@', '0', '0', 'D', '2', '0', '0', '\r' };
	char* buffer4;
	char* buffer5;
	
	char input[4];
	//getInput(input);
	getInputK(driverFp,input);

  int i = 0;

  //initialise device ports
  writeToDriver(driverFp, buffer1);
  writeToDriver(driverFp, buffer2);
  writeToDriver(driverFp, buffer3);
  //getchar();
  int timer = 0;

  //switch between 7seg 1,2,3,4
  do
  {
	  for (i=1; i<=4; i++)
	  {
		switch (i)
		{
			case 1: buffer4 = COLUMN_1;
				buffer5 = setDisplayValue(input[i-1]);
				break;
			case 2: buffer4 = COLUMN_2;
				buffer5 = setDisplayValue(input[i-1]);
				break;
			case 3: buffer4 = COLUMN_3;
				buffer5 = setDisplayValue(input[i-1]);
				break;
			case 4: buffer4 = COLUMN_4;
				buffer5 = setDisplayValue(input[i-1]);
				break;
		}
		writeToDriver(driverFp, buffer5);
		writeToDriver(driverFp, buffer4);
		usleep(6500);
	  }
	  timer ++;
  }
  while (timer < 100);

}

char readKeys(int driverFp)
{
	char buffer1[8] = { '@', '0', '0', 'D', '0', '0', '0', '\r' };
	char buffer2[8] = { '@', '0', '0', 'D', '1', 'F', 'F', '\r' };
	char buffer3[8] = { '@', '0', '0', 'D', '2', 'F', 'F', '\r' };
	char* buffer4;
	char* buffer5;
	char button;

	int i;

	  //initialise device ports
	  writeToDriver(driverFp, buffer1);
	  writeToDriver(driverFp, buffer2);
	  writeToDriver(driverFp, buffer3);

	  //switch between 7seg 1,2,3,4
	  do
	  {
		  for (i=0; i<4; i++)
		  {
			switch (i)
			{
				case 0: buffer4 = COLUMN_1;
					break;
				case 1: buffer4 = COLUMN_2;
					break;
				case 2: buffer4 = COLUMN_3;
					break;
				case 3: buffer4 = COLUMN_4;
					break;
			}

			writeToDriver(driverFp, buffer4);
			int butNo = writeToDriver(driverFp, CHECK_BUTTON);
			if (butNo != 0)
			{
				return keypadMapping[(int)log2(butNo)][i];
			}

		  }
	  }
	  while (1);

	  return -1;
}

void getInput(char* input[4])
{	
	int i;

	for (i = 1; i <= 4; i++)
	{
		printf("Enter Value for Column %d (0-F): ", i);	
		input[i-1] = malloc(sizeof(DISPLAY_0));			
		scanf("%s", input[i-1]);
		printf ("%s",input[i-1]);			
	}
}
void getInputK(int fd, char* input)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		printf("Press Key (%d):", i+1);
		input[i] = readKeys(fd);
		usleep(200000);
	}
}

char* setDisplayValue(char charSelect)
{
	switch (charSelect)
	{
		case '0': return(DISPLAY_0); 
			break;
		case '1': return(DISPLAY_1); 
			break;
		case '2': return(DISPLAY_2); 
			break;
		case '3': return(DISPLAY_3); 
			break;
		case '4': return(DISPLAY_4); 
			break;
		case '5': return(DISPLAY_5); 
			break;
		case '6': return(DISPLAY_6); 
			break;
		case '7': return(DISPLAY_7); 
			break;
		case '8': return(DISPLAY_8); 
			break;
		case '9': return(DISPLAY_9); 
			break;
		case 'A': return(DISPLAY_A); 
			break;
		case 'B': return(DISPLAY_B); 
			break;
		case 'C': return(DISPLAY_C); 
			break;
		case 'D': return(DISPLAY_D); 
			break;
		case 'E': return(DISPLAY_E); 
			break;
		case 'F': return(DISPLAY_F); 
			break;
	}
}

int writeToDriver(int fd, char* buffer)
{
  int ret;
  char buffer2[8];

  ret = write (fd,buffer, sizeof(buffer));
  if (ret < 0)
  {
	  printf("Write ret val = %d error num = %d err: %s\n", ret, errno,strerror(errno));
	  return -1;
  }
  ret = read(fd, &buffer2, sizeof(buffer2));
  if (ret < 0)
  {
	  printf("Read ret val = %d error num = %d err: %s\n", ret, errno,strerror(errno));
	  return -1;
  }
  //printf("\n---> %d", (int)buffer2[4] - '0');
  return (int)buffer2[4] - '0';

}
