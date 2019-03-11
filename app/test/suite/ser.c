/*************************************************************************
    > File Name: main.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年11月19日 星期一 15时20分08秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define SERVER_PORT 2430
#define BUF_LEN 512


void udp_msg_sender(int fd, int num, struct sockaddr *dst)
{
	int i;
	socklen_t len;
	struct sockaddr_in src;
	char r_buf[20] = {0};
	char w_buf[20] = {0x53, 0x69, 0x75, 0x64, 0x69, 0x5f, 0x37, 0x42, 0x6d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

	printf("***************\n");
	w_buf[10] = num;

	len = sizeof(w_buf);
	for(i = 0; i < len; i ++)
		printf("0x%x ", w_buf[i]);
	printf("\nlen: %d\n", len);

	sendto(fd, w_buf, BUF_LEN, 0, dst, len);

	printf("################\n");
	memset(r_buf, 0, sizeof(r_buf));
	recvfrom(fd, r_buf, BUF_LEN, 0, (struct sockaddr *)&src, &len);
	len = sizeof(r_buf);
	for(i = 0; i < len; i ++)
		printf("0x%x ", r_buf[i]);
	printf("\nlen: %d\n", len);

	return;

}

int main(int argc, char **argv)
{
	int num, ret;
	int server_fd;
	struct sockaddr_in ser_addr;


	if(argc != 2){
		printf("Usage: %s num(1~5)\n", argv[0]);
		return -1;
	}
	num = atoi(argv[1]);
	
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(server_fd < 0){
		printf("creat socket fail!\n");
		return -1;
	}

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(SERVER_PORT);

	ret = bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(ret < 0){
		printf("socket bind fail!\n");
		return -1;
	}

	udp_msg_sender(server_fd, num, (struct sockaddr *)&ser_addr);

	//udp_msg_recver(fd);


	close(server_fd);
	return 0;
}
