/*************************************************************************
    > File Name: test.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年05月25日 星期五 10时54分08秒
 ************************************************************************/

#include <stdio.h>

#include "./main.h"

int main(void)
{
	char w_buf[32] = {0};
	char r_buf[128] = {0};

	sprintf(w_buf, "AT_CHECK0");
	rs485_write(w_buf);

	rs485_read(r_buf);

	return 0 ;
}
