/*************************************************************************
    > File Name: server.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月16日 星期一 14时29分26秒
 ************************************************************************/
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>

#define PORT 6464
#define IP "192.168.1.101"
//#define IP "127.0.0.1"

int main(void)
{
	int ret;
	int ser_fd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	int sin_size;
	char buf[20] = {0};

	ser_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(ser_fd < 0){
		perror("socket error!\n");
		return -1;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;		//inet
	server_addr.sin_port = htons(PORT);	//端口号
	//server_addr.sin_port = htonl(PORT);	//端口号
	server_addr.sin_addr.s_addr = inet_addr(IP);	//本地回环

	ret = bind(ser_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(ret == -1){
		perror("bind error!\n");
		return -1;
	}

	listen(ser_fd, 5);
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);
		new_fd = accept(ser_fd, (struct sockaddr *)&server_addr, &sin_size);
		if(new_fd < 0 ){
			printf("new client failed!\n");
		}else{
			printf("new client success!\n");
			while(1)
			{
				printf("open or close:");
				scanf("%s", buf);
				printf("buf: %s\n", buf);
				ret = strncmp(buf, "quit", strlen("quit"));
				if(ret == 0)
					break;
				send(new_fd, buf, sizeof(buf), 0);
			}
		}
	}



	close(ser_fd);
	return 0;
}
