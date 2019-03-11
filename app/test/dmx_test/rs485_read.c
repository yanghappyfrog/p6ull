#include <stdio.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h> /* TIOCGRS485 + TIOCSRS485 ioctl definitions */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "./main.h"

/******************************************************************************
 * NAME:
 * read_data
 *
 * DESCRIPTION:
 * read data to serial port
 *
 *
 * PARAMETERS:
 * fd - The fd of serial port to write
 * *buf - The buffer to keep data
 * len - The count of data
 * time - time out
 *
 * RETURN:
 * Count of data wrote
 ******************************************************************************/
int read_data(int fd, char *buf, int len, int time_ms)
{
	int nfds;
	int nread = 0;
	unsigned char read_byte = 0;
	char read_temp[512] = {0};
	fd_set readfds;
	struct timeval tv;
	char *p;

	//printf("***read com data***\n");

	tv.tv_sec = time_ms/1000;
	tv.tv_usec = time_ms%1000;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	bzero(read_temp, sizeof(read_temp));
	nfds = select(fd+1, &readfds, NULL, NULL, &tv);

	if(nfds == 0) 
	{
		printf("timeout!\r\n");
		return -1;
	}else{
		nread = read(fd, &read_byte, sizeof(char));//
		if(nread<=0){
			printf("read get problem[%d]!\r\n", nread);
			return -1;
		}
		//printf("recv data: %c\n", read_byte);
		if(read_byte == '<'){
			//printf("data start...\n");
			p = read_temp;
			strcpy(p, &read_byte);
			*p++;
			while(1)
			{
				nread = read(fd, &read_byte, sizeof(char));//
				if(nread<=0){
					printf("read get problem[%d]!\r\n", nread);
					return -1;
				}
				//printf("recv data: %c\n", read_byte);

				if(read_byte == '>'){
					//printf("data over...\n");
					strcpy(p, &read_byte);
					break;
				}
				strcpy(p, &read_byte);
				*p++;
			}
			//printf("recv data: %s\n", read_temp);
			memcpy(buf, read_temp, strlen(read_temp));
			return 0;
		}else{
			printf("data start error!\n");
			return -1;
		}
	}

}

int rs485_read(int channel, char *r_buf)
{
	int fd_gpio, ret;
	int fd, i, len, nread,r;
	struct termios oldtio;	
	int speed ;
	int spee_flag = 0, device_flag = 0;
	char ch[2] = {0};

	printf("*** rs485_read *** \n");
	fd_gpio = open("/dev/uart_gpio0", O_RDWR);
	if(fd_gpio < 0){
		perror("open failed\n");
		close(fd_gpio);
		return -1;
	}

	/* open serial port */
	fd = open_port(TTY_DEV);
	if (fd < 0) {
		perror("open failed");
		close(fd);
		close(fd_gpio);
		return -1;
	}
	
	memset(&oldtio, 0, sizeof(oldtio));
	/* save the old serial port configuration */
	if(tcgetattr(fd, &oldtio) != 0) {
		perror("set_port/tcgetattr");
		close(fd);
		close(fd_gpio);
		return -1;	
	}
	
	rs485_enable(fd,ENABLE);	

	/* set serial port */
	i = set_port(fd, 115200, 8, 'N', 1);
	if (i < 0) {
		perror("set_port failed");
		close(fd);
		close(fd_gpio);
		return -1;	
	}

	sprintf(ch, "%d", channel);
	printf("ch: %s\n", ch);
	write(fd_gpio, ch, strlen(ch));

	ret = read_data(fd, r_buf, strlen(r_buf), 20*1000);
	if(ret < 0){
		printf(" ex%d no device !!\n", channel);
		close(fd);
	}
	printf("ex%d recv data: %s\n", channel, r_buf);

	/* restore the old configuration */		
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd_gpio);
	close(fd);
	return 0;
}
