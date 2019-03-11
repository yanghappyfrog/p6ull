#include <stdio.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
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
	int fd, i, len, nread,r;
	struct termios oldtio;	
	int speed ;
	int spee_flag = 0, device_flag = 0;
	char ch[2] = {0};

	printf("*** rs485_write *** \n");
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
	printf("ch: %s, w_buf: %s\n", ch, w_buf);
	write(fd_gpio, ch, strlen(ch));
	write(fd, w_buf, strlen(w_buf));

	usleep(10*1000);
	/* restore the old configuration */		
	tcsetattr(fd, TCSANOW, &oldtio);

	close(fd_gpio);
	close(fd);
	return 0;
}
