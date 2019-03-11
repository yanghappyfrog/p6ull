/*************************************************************************
    > File Name: com_init.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年12月28日 星期四 18时47分19秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <time.h>

#define UART_DEV "/dev/ttyO3"
#define UART_GPIO "/dev/uart_gpio0"

static int speed[] = 
{
	921600, 460800, 230400, 115200, 
	57600,  38400,  19200,  9600, 
	4800,   2400,   1800,   1200,
	600,    300,    150,    110, 
	75,     50
};

static int baudflag[] = 
{
	B921600, B460800, B230400, B115200, 
	B57600,  B38400,  B19200,  B9600, 
	B4800,   B2400,   B1800,   B1200,
	B600,    B300,    B150,    B110, 
	B75,     B50
};

/******************************************************************************
 * NAME:
 * UartSpeed2Flag
 *
 * DESCRIPTION:
 * Translate baudrate into flag
 *
 * PARAMETERS:
 * speedpar - The baudrate to convert
 *
 * RETURN:
 * The flag
 ******************************************************************************/
static int UartSpeed2Flag(int speedpar)
{
	int i;
	int cnt = 0;

	for (i = 0; i < sizeof(speed)/sizeof(int); i++) {
		cnt = (speedpar - speed[i]);
		if ( cnt == 0) 
			return baudflag[i];
	}

	return B9600;
}

/******************************************************************************
 * NAME:
 * stup_port
 *
 * DESCRIPTION:
 * Set serial port (include baudrate, line control)
 *
 * PARAMETERS:
 * fd - The fd of serial port to setup
 * baud - Baudrate: 9600, ...
 * databits - Databits: 5, 6, 7, 8
 * parity - Parity: 0(None), 1(Odd), 2(Even)
 * stopbits - Stopbits: 1, 2
 *
 * RETURN:
 * 0 for OK; Others for ERROR
 ******************************************************************************/
int setup_port(int fd, int baud, int databits, int parity, int stopbits)
{
	struct termio term_attr;
	struct termio oterm_attr;

	/* Get current setting */
	if (ioctl(fd, TCGETA, &term_attr) < 0) {
		printf("TCGETA fail\n");	

		return -1;
	}

	/* Backup old setting */
	memcpy(&oterm_attr, &term_attr, sizeof(struct termio));

	term_attr.c_iflag &= ~(INLCR | IGNCR | ICRNL | ISTRIP| IXON);
	term_attr.c_oflag &= ~(OPOST | ONLCR | OCRNL);
	term_attr.c_lflag &= ~(ISIG | ECHO | ICANON | NOFLSH);
	term_attr.c_cflag &= ~CBAUD;
	term_attr.c_cflag |= CREAD | UartSpeed2Flag(baud);

	/* Set databits */
	term_attr.c_cflag &= ~(CSIZE);
	switch (databits) {
		case 5:
			term_attr.c_cflag |= CS5;
			break;

		case 6:
			term_attr.c_cflag |= CS6;
			break;

		case 7:
			term_attr.c_cflag |= CS7;
			break;

		case 8:
		default:
			term_attr.c_cflag |= CS8;
			break;
	}

	/* Set parity */
	switch (parity) {
		case 1: /* Odd parity */
			term_attr.c_cflag |= (PARENB | PARODD);
			break;

		case 2: /* Even parity */
			term_attr.c_cflag |= PARENB;
			term_attr.c_cflag &= ~(PARODD);
			break;

		case 0: /* None parity */
		default:
			term_attr.c_cflag &= ~(PARENB);
			break;
	}


	/* Set stopbits */
	switch (stopbits) {
		case 2: /* 2 stopbits */
			term_attr.c_cflag |= CSTOPB;
			break;

		case 1: /* 1 stopbits */
		default:
			term_attr.c_cflag &= ~CSTOPB;
			break;
	}

	term_attr.c_cc[VMIN] = 1;
	term_attr.c_cc[VTIME] = 0;

	if (ioctl(fd, TCSETAW, &term_attr) < 0) {
		printf("tcsetaw fail\n");
		return -1;
	}

	if (ioctl(fd, TCFLSH, 2) < 0) {
		printf("tcflsh fail\n");	
		return -1;
	}

	return 0;
}

/******************************************************************************
 * NAME:
 * write_data
 *
 * DESCRIPTION:
 * Write data to serial port
 *
 *
 * PARAMETERS:
 * fd - The fd of serial port to write
 * buf - The buffer to keep data
 * len - The count of data
 *
 * RETURN:
 * Count of data wrote
 ******************************************************************************/
int write_data(int fd, char *buf, int len)
{
	int ret;

	ret = write(fd, buf, len);
	if(ret < 1){
		return -1;
	}
	return 0;
}

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
			//printf("data star...\n");
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
		}else{
			printf("data star error!\n");
			return -1;
		}
	}

}


#if 0
int main(int argc, char **argv)
{
	unsigned long temp = 0;
	int i, ret, len, crc;
	int gpio_fd = 0;
	int uart_fd = 0;
	int addr = 0;
	char ch = 0;
	char buff[256] = {0};
	char w_buf[256] = "K_1111111111111111_OK";
	unsigned char *ptr = w_buf;

	int baud = 9600;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;


	if(argc != 2){
		printf("Usage: %s 0~12\n", argv[0]);
		return -1;
	}

	gpio_fd = open(UART_GPIO, O_RDWR, 0);
	if(gpio_fd < 0){
		printf(" Open gpio_fd dev errir!\n");
		return -1;
	}

	//init uart (ttyO3)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		return -1;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		return -1;
	}

	//set gpio for send
	printf("### e_com1 send...\n");
	addr = atoi(argv[1]);
	if(addr < 0 || addr >12){
		printf("Usage: %s 0~12\n", argv[0]);
		return -1;
	}
	printf(" @@@@ addr: %d @@@@\n", addr);
	write(gpio_fd, argv[1], strlen(argv[1]));

	/* crc */
	//len=strlen(w_buf);
	//while(len--) {  
	//	for(i = 0x80; i != 0; i = i >> 1) {  
	//		temp = temp * 2;  
	//		if((temp & 0x10000) != 0)  
	//			temp = temp ^ 0x11021;  

	//		if((*ptr & i) != 0)   
	//			temp = temp ^ (0x10000 ^ 0x11021);  
	//	}  
	//	*ptr++;  
	//}  
	//crc = temp;
	//printf("0x%x\n ",crc);  

	//ret = write(uart_fd, w_buf, strlen(w_buf));

	sprintf(w_buf, "AT+CHECK%d", 0);
	strcat(w_buf, &ch);

	write_data(uart_fd, w_buf, strlen(w_buf));

	

	close(gpio_fd);
	close(uart_fd);
	return 0;
}
#endif
