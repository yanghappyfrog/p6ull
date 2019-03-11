/*************************************************************************
    > File Name: rs485.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年05月28日 星期一 10时02分26秒
 ************************************************************************/
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

typedef enum {DISABLE = 0, ENABLE} RS485_ENABLE_t;

int rs485_write(int channel, char *w_buf);
int rs485_read(int channel, char *r_buf);
int set_port(int fd, int nSpeed, int nBits, char nEvent, int nStop);
int open_port(char *dir);
int rs485_enable(const int fd, const RS485_ENABLE_t enable);

