/*************************************************************************
    > File Name: key.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 13时59分38秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>  
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "./other_gpio.h"
#include "../main.h"

#define BUFSZ 32
//#define PORT 6464
//#define IP "127.0.0.1"
#define KEYPAD_DEV "/dev/keypad0"
//#define BUZZER_DEV "/dev/other_gpio0"

//struct sockaddr_in address;  
int keypad_fd;

void input_handler(int num)  
{  
	//int buz_fd;
	char data;  
	int len;  
	int key_val = 0;
	char buf[BUFSZ] = {0};

	//buz_fd = open(BUZZER_DEV, O_RDWR, 0);
	//if(buz_fd < 0){
	//	perror("open buzzer error\n");
	//	return ;
	//}

	ioctl(gval.buz_fd, IOREQ_BUZZER_ON);
	key_val = 0;
	read(keypad_fd, &key_val, sizeof(key_val));
	printf("client[read key val: 0x%x]\n", key_val);
	sprintf(buf, "KeyPressed value:0x%x\n", key_val);
	sendto(gval.clifd, &buf, sizeof(buf), 0, (struct sockaddr *)&gval.address, sizeof(gval.address));		
	usleep(50*1000);
	ioctl(gval.buz_fd, IOREQ_BUZZER_OFF);

	//close(buz_fd);
}  

/*
 * 读取按键值并通过socket接口发送给应用层
 */
//int main(void)
void *send_key_val(void *a)
{
	int ret, sin_len, oflags;

	printf("*** key pthread *** \n");
	keypad_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(keypad_fd < 0){
		printf("open dev error!\n");
		return ;
	}

	signal(SIGIO, input_handler);
	fcntl(keypad_fd, F_SETOWN, getpid());
	oflags = fcntl(keypad_fd, F_GETFL);  
	fcntl(keypad_fd, F_SETFL, oflags | FASYNC);  

//	bzero(&address, sizeof(address));  
//	address.sin_family = AF_INET;
//	address.sin_port = htons(PORT);
//	address.sin_addr.s_addr = inet_addr(IP);
//	sin_len = sizeof(address);
//
//	clifd = socket(AF_INET, SOCK_DGRAM, 0);
//	if(-1 == clifd){
//		perror("socket error!\n");
//		return ;
//	}
	//bind(clifd, (struct sockaddr *)&address, sizeof(address));

	while(1)
	{
		sleep(1);
	}
}


