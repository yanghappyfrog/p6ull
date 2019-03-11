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
	int i;
	int fd = 0;
	int val;

	if(argc != 2){
		printf("Usage: %s 1/2  1:cold; 2:hot \n", argv[0]);
		return -1;
	}

	i = atoi(argv[1]);

	fd = open(DEV, O_RDWR, 0);
	if(fd < 0){
		printf(" Open dev error!\n");
		return -1;
	}

	/* 3G hot reset */
	if(i == 2)
	{
		printf("hot reset...\n");
		ioctl(fd, IOREQ_CARDA_2_OFF);
		getchar();
		usleep(100*1000);
		ioctl(fd, IOREQ_CARDA_2_EN);
	}

	/* 3G cold reset */
	if(i == 1)
	{
		while(1)
		{
		printf("cold reset...\n");
		ioctl(fd, IOREQ_CARDA_1_OFF);
		//getchar();
		sleep(5);
		ioctl(fd, IOREQ_CARDA_1_EN);
		sleep(5);
		}
	}

	//reset keypad
	//ioctl(fd, IOREQ_RESET_KEYPAD_L);
	//usleep(10*1000);
	//ioctl(fd, IOREQ_RESET_KEYPAD_H);


	//val = atoi(argv[1]);
	//printf("val: %d\n", val);
	//switch(val)
	//{
	//	case 0:
	//		ioctl(fd, IOREQ_BUZZER_OFF);
	//		break;
	//	case 1:
	//		ioctl(fd, IOREQ_BUZZER_ON);
	//		break;
	//	default:
	//		printf("argv error!\n");
	//		return -1;
	//}

	getchar();
	close(fd);
	return 0;
}
