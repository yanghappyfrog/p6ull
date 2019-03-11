#include "./serial.h"

#define E_UART "/dev/ttymxc1"	//for uart2 ex_uart 
#define I_UART "/dev/ttymxc2"	//for uart3 in_uart

/**
 * @brief: set the properties of serial port 
 * @Param: fd: file descriptor
 * @Param: nSpeed: Baud Rate
 * @Param: nBits: character size
 * @Param: nEvent: parity of serial port
 * @Param: nStop: stop bits
 */
int set_port(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio;
	
	
	memset(&newtio, 0, sizeof(newtio));
	/* ignore modem control lines and enable receiver */
	newtio.c_cflag = newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;	
	/* set character size */
	switch (nBits) {
		case 8: 
			newtio.c_cflag |= CS8;
			break;
		case 7: 
			newtio.c_cflag |= CS7;
			break;
		case 6: 
			newtio.c_cflag |= CS6;
			break;
		case 5: 
			newtio.c_cflag |= CS5;
			break;
		default:
			newtio.c_cflag |= CS8; 
			break;
	}
	/* set the parity */
	switch (nEvent) {
		case 'o':
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'e':
		case 'E':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'n':
		case 'N':
			newtio.c_cflag &= ~PARENB;
			break;
		default:
			newtio.c_cflag &= ~PARENB; 
			break;
	}
	/* set the stop bits */
	switch (nStop) {
		case 1:
			newtio.c_cflag &= ~CSTOPB;
			break;
		case 2:
			newtio.c_cflag |= CSTOPB;
			break;
		default:
			newtio.c_cflag &= ~CSTOPB; 
			break;	
	}	
	/* set output and input baud rate */
	switch (nSpeed) {
		case 0:
			cfsetospeed(&newtio, B0);
			cfsetispeed(&newtio, B0);
			break;
		case 50:
			cfsetospeed(&newtio, B50);
			cfsetispeed(&newtio, B50);
			break;
		case 75:
			cfsetospeed(&newtio, B75);
			cfsetispeed(&newtio, B75);
			break;
		case 110:
			cfsetospeed(&newtio, B110);
			cfsetispeed(&newtio, B110);
			break;
		case 134:
			cfsetospeed(&newtio, B134);
			cfsetispeed(&newtio, B134);
			break;
		case 150:
			cfsetospeed(&newtio, B150);
			cfsetispeed(&newtio, B150);
			break;
		case 200:
			cfsetospeed(&newtio, B200);
			cfsetispeed(&newtio, B200);
			break;
		case 300:
			cfsetospeed(&newtio, B300);
			cfsetispeed(&newtio, B300);
			break;
		case 600:
			cfsetospeed(&newtio, B600);
			cfsetispeed(&newtio, B600);
			break;
		case 1200:
			cfsetospeed(&newtio, B1200);
			cfsetispeed(&newtio, B1200);
			break;
		case 1800:
			cfsetospeed(&newtio, B1800);
			cfsetispeed(&newtio, B1800);
			break;
		case 2400:
			cfsetospeed(&newtio, B2400);
			cfsetispeed(&newtio, B2400);
			break;
		case 4800:
			cfsetospeed(&newtio, B4800);
			cfsetispeed(&newtio, B4800);
			break;
		case 9600:
			cfsetospeed(&newtio, B9600);
			cfsetispeed(&newtio, B9600);
			break;
		case 19200:
			cfsetospeed(&newtio, B19200);
			cfsetispeed(&newtio, B19200);
			break;
		case 38400:
			cfsetospeed(&newtio, B38400);
			cfsetispeed(&newtio, B38400);
			break;
		case 57600:
			cfsetospeed(&newtio, B57600);
			cfsetispeed(&newtio, B57600);
			break;
		case 115200:
			cfsetospeed(&newtio, B115200);
			cfsetispeed(&newtio, B115200);
			break;
		case 230400:
			cfsetospeed(&newtio, B230400);
			cfsetispeed(&newtio, B230400);
			break;
		default:
			cfsetospeed(&newtio, B115200);
			cfsetispeed(&newtio, B115200);
			break;	
	}
	/* set timeout in deciseconds for non-canonical read */
	newtio.c_cc[VTIME] = 0;
	/* set minimum number of characters for non-canonical read */
	newtio.c_cc[VMIN] = 0;
	/* flushes data received but not read */
	tcflush(fd, TCIFLUSH);
	/* set the parameters associated with the terminal from 
		the termios structure and the change occurs immediately */
	if((tcsetattr(fd, TCSANOW, &newtio))!=0)
	{
		perror("set_port/tcsetattr");
		return -1;
	}
	
	return 0;
}

/**
 * @brief: open serial port 
 * @Param: dir: serial device path
 */
int open_port(char *dir)
{
	int fd ;
	fd = open(dir, O_RDWR);
	if(fd < 0) {
		perror("open_port");	
	}	
	return fd;
}

/**
 * @brief: main function  
 * @Param: argc: number of parameters
 * @Param: argv: parameters list
 */

int rs485_enable(const int fd, const RS485_ENABLE_t enable)
{
        struct serial_rs485 rs485conf;
        int res;

        /* Get configure from device */
        res = ioctl(fd, TIOCGRS485, &rs485conf);
        if (res < 0) {
                perror("Ioctl error on getting 485 configure:");
                close(fd);
                return res;
        }

        /* Set enable/disable to configure */
        if (enable) {   // Enable rs485 mode
                rs485conf.flags |= SER_RS485_ENABLED;
        } else {        // Disable rs485 mode
                rs485conf.flags &= ~(SER_RS485_ENABLED);
        }

        rs485conf.delay_rts_before_send = 0x00000004;

        /* Set configure to device */
        res = ioctl(fd, TIOCSRS485, &rs485conf);
        if (res < 0) {
                perror("Ioctl error on setting 485 configure:");
                close(fd);
        }

        return res;
}

int rs485_write(int channel, char *w_buf)
{
	int fd_gpio;
	int fd_uart, i, len, nread,r;
	struct termios oldtio;	
	int speed ;
	int spee_flag = 0, device_flag = 0;
	char ch[2] = {0};

	//printf("*** rs485_write *** \n");
	fd_gpio = open("/dev/uart_gpio0", O_RDWR);
	if(fd_gpio < 0){
		perror("open failed\n");
		close(fd_gpio);
		return -1;
	}

	/* open serial port */
	fd_uart = open_port(E_UART);
	if (fd_uart < 0) {
		perror("open failed");
		close(fd_uart);
		close(fd_gpio);
		return -1;
	}

	memset(&oldtio, 0, sizeof(oldtio));
	/* save the old serial port configuration */
	if(tcgetattr(fd_uart, &oldtio) != 0) {
		perror("set_port/tcgetattr");
		close(fd_uart);
		close(fd_gpio);
		return -1;	
	}
	
	rs485_enable(fd_uart, ENABLE);	
	
	/* set serial port */
	i = set_port(fd_uart, 115200, 8, 'N', 1);
	if (i < 0) {
		perror("set_port failed");
		close(fd_uart);
		close(fd_gpio);
		return -1;	
	}

	sprintf(ch, "%d", channel);
	//printf("ch: %s, w_buf: %s\n", ch, w_buf);
	write(fd_gpio, ch, strlen(ch));
	write(fd_uart, w_buf, strlen(w_buf));

	usleep(10*1000);
	/* restore the old configuration */		
	tcsetattr(fd_uart, TCSANOW, &oldtio);

	close(fd_gpio);
	close(fd_uart);
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

int rs485_read(int channel, char *r_buf, int time)
{
	int fd_gpio, ret;
	int fd_uart, i, nread,r;
	struct termios oldtio;	
	int speed ;
	int spee_flag = 0, device_flag = 0;
	char ch[2] = {0};

	//printf("*** rs485_read *** \n");
	fd_gpio = open("/dev/uart_gpio0", O_RDWR);
	if(fd_gpio < 0){
		perror("open failed\n");
		close(fd_gpio);
		return -1;
	}

	/* open serial port */
	fd_uart = open_port(E_UART);
	if (fd_uart < 0) {
		perror("open failed");
		close(fd_uart);
		close(fd_gpio);
		return -1;
	}
	
	memset(&oldtio, 0, sizeof(oldtio));
	/* save the old serial port configuration */
	if(tcgetattr(fd_uart, &oldtio) != 0) {
		perror("set_port/tcgetattr");
		close(fd_uart);
		close(fd_gpio);
		return -1;	
	}
	
	rs485_enable(fd_uart, ENABLE);	

	/* set serial port */
	i = set_port(fd_uart, 115200, 8, 'N', 1);
	if (i < 0) {
		perror("set_port failed");
		close(fd_uart);
		close(fd_gpio);
		return -1;	
	}

	sprintf(ch, "%d", channel);
	write(fd_gpio, ch, strlen(ch));

	ret = read_data(fd_uart, r_buf, strlen(r_buf), time*1000);
	if(ret < 0){
		//printf(" ex%d no device !!\n", channel);
		close(fd_gpio);
		close(fd_uart);
		return -1;
	}
	//printf("ex%d recv data: %s\n", channel, r_buf);

	/* restore the old configuration */		
	tcsetattr(fd_uart, TCSANOW, &oldtio);
	close(fd_gpio);
	close(fd_uart);
	return 0;
}

int rs232_write(char *w_buf)
{
	int fd_uart, i, len, nread,r;
	struct termios oldtio;	
	int speed ;
	int spee_flag = 0, device_flag = 0;

	//printf("*** rs232_write *** \n");

	/* open serial port */
	fd_uart = open_port(I_UART);
	if (fd_uart < 0) {
		perror("open failed");
		close(fd_uart);
		return -1;
	}

	memset(&oldtio, 0, sizeof(oldtio));
	/* save the old serial port configuration */
	if(tcgetattr(fd_uart, &oldtio) != 0) {
		perror("set_port/tcgetattr");
		close(fd_uart);
		return -1;	
	}
	
	/* set serial port */
	i = set_port(fd_uart, 115200, 8, 'N', 1);
	if (i < 0) {
		perror("set_port failed");
		close(fd_uart);
		return -1;	
	}

	write(fd_uart, w_buf, strlen(w_buf));

	usleep(10*1000);
	/* restore the old configuration */		
	tcsetattr(fd_uart, TCSANOW, &oldtio);

	close(fd_uart);
	return 0;
}

int rs232_read(char *r_buf)
{
	int ret, fd_uart, i, nread,r;
	struct termios oldtio;	
	int speed ;
	int spee_flag = 0, device_flag = 0;

	//printf("*** rs232_read *** \n");
	/* open serial port */
	fd_uart = open_port(I_UART);
	if (fd_uart < 0) {
		perror("open failed");
		close(fd_uart);
		return -1;
	}

	memset(&oldtio, 0, sizeof(oldtio));
	/* save the old serial port configuration */
	if(tcgetattr(fd_uart, &oldtio) != 0) {
		perror("set_port/tcgetattr");
		close(fd_uart);
		return -1;	
	}

	/* set serial port */
	i = set_port(fd_uart, 115200, 8, 'N', 1);
	if (i < 0) {
		perror("set_port failed");
		close(fd_uart);
		return -1;	
	}

	ret = read_data(fd_uart, r_buf, strlen(r_buf), 2*1000);
	if(ret < 0){
		//printf("no device !!\n");
		close(fd_uart);
		return -1;
	}
	//printf("recv data: %s\n", r_buf);

	tcsetattr(fd_uart, TCSANOW, &oldtio);
	close(fd_uart);
	return 0;
}
