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

int main(int argc, char **argv)
{
	char w_buf[32] = {0};
	char r_buf[128] = {0};
	int channel = 8;

	sprintf(w_buf, "!SWITCH_HAND;");
	printf("w_buf: %s\n", w_buf);
	rs485_write(2, w_buf);

	while(1)
	{
		sprintf(w_buf, "!SWITCH_B;");
		printf("w_buf: %s\n", w_buf);
		rs485_write(2, w_buf);
		sleep(5);

		sprintf(w_buf, "!SWITCH_C;");
		printf("w_buf: %s\n", w_buf);
		rs485_write(2, w_buf);
		sleep(5);

		sprintf(w_buf, "!SWITCH_OFF;");
		printf("w_buf: %s\n", w_buf);
		rs485_write(2, w_buf);
		sleep(5);

	}
	//	rs485_read(1, r_buf);
	//	printf("r_buf: %s\n", r_buf);

	//	while(1)
	//	{
	//		bzero(w_buf, strlen(w_buf));
	//		bzero(r_buf, strlen(r_buf));
	//		sprintf(w_buf, "<AT_CHECK0>");
	//		rs485_write(channel, w_buf);
	//		rs485_read(channel, r_buf);
	//		sleep(2);
	//
	//		bzero(w_buf, strlen(w_buf));
	//		bzero(r_buf, strlen(r_buf));
	//		//sprintf(w_buf, "<AT_CHECK0_LUX_GPS_DATA>");
	//		sprintf(w_buf, "<AT_CHECK0_IDDATA>");
	//		rs485_write(channel, w_buf);
	//		rs485_read(channel, r_buf);
	//		sleep(2);
	//	}

	return 0 ;
}
