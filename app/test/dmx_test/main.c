/*************************************************************************
    > File Name: test.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年05月25日 星期五 10时54分08秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "./main.h"

int main(void)
{
	char w_buf[32] = {0};
	char r_buf[128] = {0};
	int channel = 8;

	sprintf(w_buf, "<AT_CHECK0>");
	rs485_write(1, w_buf);
	printf("w_buf: %s\n", w_buf);
	//rs485_read(1, r_buf);
	//printf("r_buf: %s\n", r_buf);

	return 0 ;
}
