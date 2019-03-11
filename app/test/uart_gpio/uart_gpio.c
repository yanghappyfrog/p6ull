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


#define DEV "/dev/uart_gpio0"

int main(int argc, char **argv)
{
	int i;
	int fd = 0;
	int val;

	fd = open(DEV, O_RDWR, 0);
	if(fd < 0){
		printf(" Open dev error!\n");
		return -1;
	}
	write(fd, "2", strlen("2"));

	close(fd);
	return 0;
}
