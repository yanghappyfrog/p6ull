#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "./serial.h"


int main(void)
{	
	int baud = 115200;
	int databits = 8;
	int stopbits = 1;
	//int parity = 0;
	char parity = 'N';
	int lte_fd, ret;
	char buf[20] = {0};
	char read_buf[100] = {0};
	int signal = 0;
	int fd = 0;
	char *p = NULL;

	lte_fd = open("/dev/ttyUSB2", O_RDWR);
	if(lte_fd < 0){
		printf("open dev error!\n");
		return -1;
	}

	ret = set_port(lte_fd, baud, databits, parity, stopbits);
	if(ret < 0){
		printf("setup port error!\n");
		return -1;
	}

	strcpy(buf, "AT+CSQ\r");
	write(lte_fd, buf, strlen(buf));
	read(lte_fd, read_buf, sizeof(read_buf));
	printf("read_buf:%s\n", read_buf);
	read(lte_fd, read_buf, sizeof(read_buf));
	printf("read_buf:%s\n", read_buf);

	p = strrchr(read_buf, ':');
	sprintf(read_buf, "%s", p + 2);
	printf("read_buf:%s\n", read_buf);

	p = strchr(read_buf, ',');
	*p = '\0';
	printf("read_buf:%s\n", read_buf);
	signal = atoi(read_buf);

	return signal;
}
