/*************************************************************************
    > File Name: serial.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年12月13日 星期三 17时39分28秒
 ************************************************************************/

int speed_to_flag(int speed);
int setup_port(int fd, int baud, int databits, int parity, int stopbits);
int read_data(int fd, char *buf, int len, int time_ms);
int write_data(int fd, char *buf, int len);

