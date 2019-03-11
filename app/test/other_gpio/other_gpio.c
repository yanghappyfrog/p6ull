/*************************************************************************
    > File Name: uart_gpio_test.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年06月08日 星期四 18时47分19秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "other_gpio.h"

#define DEV "/dev/other_gpio0"

int main(int argc, char **argv)
{
	int fd = 0;
	int val;


	if(argc != 2){
		printf("Usage: %s 0/1  0:off; 1:on \n", argv[0]);
		return -1;
	}
	val = atoi(argv[1]);
	printf("val: %d\n", val);

	fd = open(DEV, O_RDWR, 0);
	if(fd < 0){
		printf(" Open dev error!\n");
		return -1;
	}

	/* RF POWER */
#if 0
	switch(val)
	{
		case 0:
			ioctl(fd, IOREQ_RF_PWR_OFF);
			break;
		case 1:
			ioctl(fd, IOREQ_RF_PWR_ON);
			break;
		default:
			printf("argv error!\n");
			return -1;
	}
#endif

	/* LTE POWER */
#if 1
	switch(val)
	{
		case 0:
			ioctl(fd, IOREQ_LTE_PWR_OFF);
			break;
		case 1:
			ioctl(fd, IOREQ_LTE_PWR_ON);
			break;
		default:
			printf("argv error!\n");
			return -1;
	}
#endif

	/* for buzzer */
#if 0
	switch(val)
	{
		case 0:
			ioctl(fd, IOREQ_BUZZER_OFF);
			break;
		case 1:
			ioctl(fd, IOREQ_BUZZER_ON);
			break;
		default:
			printf("argv error!\n");
			return -1;
	}
#endif


	getchar();
	close(fd);
	return 0;
}
