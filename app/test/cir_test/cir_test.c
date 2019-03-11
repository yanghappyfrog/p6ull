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
#include "serial.h"

#define DEV "/dev/ttymxc2"

int main(int argc, char **argv)
{
	int i, ret;
	int val = 0;
	int time = 0;
	char w_buf[64] = "<AT_CHECK0_CIRCUIT_ON/OFF_DATA_";
	char start[64] = "<AT_CHECK0_CIRCUIT_ON/OFF_DATA_";
	char r_buf[512] = {0};
	char on_buf[32] = "111111111111_";
	char off_buf[32] = "000000000000_";
	char d_buf[64] = "<AT_CHECK0_CIRCUIT_CURRENT_DATA>";
	//char d_buf[64] = "<AT_RAD_DATA>";
	char end[2] = ">";


#if 0
	if(argc != 3){
		printf("Usage: %s 1:on; 0:off\n", argv[0]);
		return -1;
	}
	
	val = atoi(argv[1]);
	time = atoi(argv[2]);

	switch(val){
		case 0:
			strcat(w_buf, off_buf);
			strcat(w_buf, argv[2]);
			strcat(w_buf, end);
			break;
		case 1:
			strcat(w_buf, on_buf);
			strcat(w_buf, argv[2]);
			strcat(w_buf, end);
			break;
		case 3:
			strcpy(w_buf, d_buf);
			break;
		default:
			printf("val error!\n");
			return -1;
	}

	printf("%s\n", w_buf);

	rs232_write(w_buf);
	usleep(10*1000);
	bzero(r_buf, sizeof(r_buf));
	ret = rs232_read(r_buf);
	printf("rece data: %s\n", r_buf);
#else
	
	while(1)
	{
		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "%s%s00500>", start, on_buf);
		printf("%s\n", w_buf);
		rs232_write(w_buf);
		sleep(10);
		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "<AT_CHECK0_CIRCUIT_CURRENT_DATA>");
		printf("%s\n", w_buf);
		rs232_write(w_buf);
		bzero(r_buf, sizeof(r_buf));
		rs232_read(r_buf);
		printf("%s\n", r_buf);

		sleep(2);

		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "%s%s00500>", start, off_buf);
		printf("%s\n", w_buf);
		rs232_write(w_buf);
		sleep(10);
		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "<AT_CHECK0_CIRCUIT_CURRENT_DATA>");
		printf("%s\n", w_buf);
		rs232_write(w_buf);
		bzero(r_buf, sizeof(r_buf));
		rs232_read(r_buf);
		printf("%s\n", r_buf);
	}

#endif


	return 0;
}
