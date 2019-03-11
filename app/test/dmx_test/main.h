/*************************************************************************
    > File Name: main.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年05月25日 星期五 11时06分00秒
 ************************************************************************/
#define TTY_DEV "/dev/ttymxc4"

typedef enum {DISABLE = 0, ENABLE} RS485_ENABLE_t;

int rs485_write(int channel, char *w_buf);
int rs485_read(int channel, char *r_buf);
int set_port(int fd, int nSpeed, int nBits, char nEvent, int nStop);
int open_port(char *dir);
int rs485_enable(const int fd, const RS485_ENABLE_t enable);

