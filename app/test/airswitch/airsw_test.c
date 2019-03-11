/*************************************************************************
    > File Name: keypad.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年12月18日 星期一 09时52分13秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "other_gpio.h"
#include "./serial.h"

#define DEV "/dev/keypad0"
#define BUZZER_DEV "/dev/other_gpio0"

int fd, buz_fd;
int status[10] = {0};

void analy_data(char data)
{
	char w_buf[128] = {0};

	if(data == 0x2f){
		if(status[0]  == 0){
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH01/1>");
			status[0] = 1;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}else{
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH01/0>");
			status[0] = 0;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}
	}
	if(data == 0x3f){
		if(status[1]  == 0){
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH02/1>");
			status[1] = 1;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}else{
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH02/0>");
			status[1] = 0;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}
	}
	if(data == 0x4f){
		if(status[2]  == 0){
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH03/1>");
			status[2] = 1;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}else{
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH03/0>");
			status[2] = 0;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}
	}
	if(data == 0x5f){
		if(status[3]  == 0){
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH04/1>");
			status[3] = 1;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}else{
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH04/0>");
			status[3] = 0;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}
	}
	if(data == 0x6f){
		if(status[4]  == 0){
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH05/1>");
			status[4] = 1;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}else{
			bzero(w_buf, sizeof(w_buf));
			sprintf(w_buf, "<AT_ONE_ADDR_10_CH05/0>");
			status[4] = 0;
			printf("send data: %s\n", w_buf);
			rs485_write(2, w_buf);
		}
	}
	return ;
}

void input_handler(int num)  
{  
	char data;  
	int len;  

	printf("**************\n");
	ioctl(buz_fd, IOREQ_BUZZER_ON);
	len = read(fd, &data, sizeof(data));  
	printf("key val: 0x%x\n", data);  
	usleep(50*1000);
	ioctl(buz_fd, IOREQ_BUZZER_OFF);

	analy_data(data);

}  

int  main(int argc, char **argv)
{
	int oflags;

	fd = open(DEV, O_RDWR, 0);
	if(fd < 0){
		printf("open dev error!\n");
		return -1;
	}
	buz_fd = open(BUZZER_DEV, O_RDWR, 0);
	if(fd < 0){
		printf("open dev error!\n");
		return -1;
	}

	signal(SIGIO, input_handler);
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);  
	fcntl(fd, F_SETFL, oflags | FASYNC);  

	while(1)
	{
		sleep(2);
	}


	
	return 0;
}
