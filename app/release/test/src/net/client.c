/*************************************************************************
    > File Name: server.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月16日 星期一 14时29分20秒
 ************************************************************************/
#include "./client.h"

int net_service(void)
{
	int ret, sin_len;
	struct sockaddr_in server_addr;
	char buf[BUFSZ];

	int ch = 0;
	char w_buf[128] = {0};
	char status[16] = {0};
	char s_time[6] = "00500";

	printf("**************** net_service pthread ***************\n");
	gdev.clifd = socket(AF_INET, SOCK_STREAM, 0);
	if(gdev.clifd < 0){
		printf("socket error!\n");
		return -1;
	}

    bzero(&server_addr, sizeof(server_addr));  
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(IP);
	sin_len = sizeof(server_addr);

	ret = connect(gdev.clifd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(ret == -1){
		printf("connect error, ret: %d!\n", ret);
		return -1;
	}else{
		printf("connect success!\n");
		while(1)
		{
			recv(gdev.clifd, buf, sizeof(buf), 0);
			printf("recv data: %s\n", buf);
			ret = strncmp(buf, "open", strlen("open"));
			if(ret == 0){
				printf("open circuit!\n");
				memset(gval.led, GREEN, sizeof(gval.led));
				ret = flash_all_led(gval.led);

				memset(w_buf, 0, sizeof(w_buf));
				memset(status, 0, sizeof(status));
				strcpy(status, "111111111111");
				sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_DATA_%s_%s>", ch, status, s_time);
				printf("w_buf: %s\n", w_buf);
				rs232_write(w_buf);
			}
			ret = strncmp(buf, "close", strlen("close"));
			if(ret == 0){
				printf("close circuit!\n");
				memset(gval.led, RED, sizeof(gval.led));
				ret = flash_all_led(gval.led);

				memset(w_buf, 0, sizeof(w_buf));
				memset(status, 0, sizeof(status));
				strcpy(status, "000000000000");
				sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_DATA_%s_%s>", ch, status, s_time);
				printf("w_buf: %s\n", w_buf);
				rs232_write(w_buf);
			}
			ret = strncmp(buf, "quit", strlen("quit"));
			if(ret == 0)
				break;
		}
	}

	return 0;
}
