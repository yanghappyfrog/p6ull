/*************************************************************************
    > File Name: main.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年10月08日 星期一 14时36分29秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int fd_export, fd_dir, fd_val;

	if(access("/sys/class/gpio/gpio3", F_OK)){
		//fd_export = open("/sys/class/gpio/export", O_RDWR, 0);
		//if(fd_export < 0){
		//	printf("open export error!\n");
		//	return -1;
		//}
		//write(fd_export, "3", strlen("3"));
		//close(fd_export);

		//fd_dir = open("/sys/class/gpio/gpio3/direction", O_RDWR, 0);
		//if(fd_dir < 0){
		//	printf("open direction error!\n");
		//	return -1;
		//}
		//write(fd_dir, "out", strlen("out"));
		//close(fd_dir);

		printf("*** create gpio3 node ***\n");
		system("echo 3 > /sys/class/gpio/export");
		sleep(1);
		system("echo out > /sys/class/gpio/gpio3/direction");

	}

	fd_val = open("/sys/class/gpio/gpio3/value", O_RDWR, 0);
	if(fd_val < 0){
		printf("open value error!\n");
		return -1;
	}

	while(1)
	{
		write(fd_val, "1", strlen("1"));
		usleep(10*1000);
		write(fd_val, "0", strlen("0"));
		sleep(10);
	}

	close(fd_val);
	return 0;
}
