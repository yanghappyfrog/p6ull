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

#define DEV "/dev/keypad0"
#define BUZZER_DEV "/dev/other_gpio0"

int fd, buz_fd;

void input_handler(int num)  
{  
	char data;  
	int len;  

	printf("**************\n");
	//ioctl(buz_fd, IOREQ_BUZZER_ON);
	len = read(fd, &data, sizeof(data));  
	printf("key val: 0x%x\n", data);  
	usleep(5*1000);
	//ioctl(buz_fd, IOREQ_BUZZER_OFF);
}  

int  main(int argc, char **argv)
{
	int oflags;
	char *w_buf = {"asdfaga"};
	char r_buf;

	fd = open(DEV, O_RDWR, 0);
	if(fd < 0){
		printf("open dev error!\n");
		return -1;
	}
	//buz_fd = open(BUZZER_DEV, O_RDWR, 0);
	//if(fd < 0){
	//	printf("open dev error!\n");
	//	return -1;
	//}
	write(fd, w_buf, strlen(w_buf));

	signal(SIGIO, input_handler);
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);  
	fcntl(fd, F_SETFL, oflags | FASYNC);  

	while(1)
	{
		sleep(2);
		read(fd, &r_buf, sizeof(char));
		printf("read val: 0x%x\n", r_buf);
	}


	
	return 0;
}
