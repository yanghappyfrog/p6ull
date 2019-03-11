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

/* 4G */
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
int setup_port(int *fd, int baud, int databits, int parity, int stopbits)
{
	struct termio term_attr;
	struct termio oterm_attr;

	/* Get current setting */
	if (ioctl(*fd, TCGETA, &term_attr) < 0) {
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

	if (ioctl(*fd, TCSETAW, &term_attr) < 0) {
		printf("tcsetaw fail\n");
		return -1;
	}

	if (ioctl(*fd, TCFLSH, 2) < 0) {
		printf("tcflsh fail\n");	
		return -1;
	}

	return 0;
}

int main(void)
{	
	int baud = 115200;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;
	int lte_fd;
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

	if(setup_port(&lte_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		return -1;
	}

#if 0
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
#endif

	strcpy(buf, "AT+CCLK?\r");
	write(lte_fd, buf, strlen(buf));
	read(lte_fd, read_buf, sizeof(read_buf));
	printf("read_buf:%s\n", read_buf);
	read(lte_fd, read_buf, sizeof(read_buf));
	printf("read_buf:%s\n", read_buf);

	p = strchr(read_buf, ':');
	sprintf(read_buf, "%s", p + 2);
	printf("read_buf:%s\n", read_buf);

	close(lte_fd);
}
