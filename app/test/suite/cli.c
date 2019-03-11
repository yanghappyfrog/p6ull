/*************************************************************************
    > File Name: cli.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年11月20日 星期二 09时36分18秒
 ************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char **argv)
{
	int sockfd;
	int len, ret, num;
	int on = 1;
	struct sockaddr_in saddr;
	char r_buf[20] = {0};
	char w_buf[20] = {0x53, 0x69, 0x75, 0x64, 0x69, 0x5f, 0x37, 0x42, 0x6d, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

	if(argc != 2){
		printf("Usage: %s num(1~5)\n", argv[0]);
		return -1;
	}

	w_buf[10] = atoi(argv[1]);

	len = sizeof(struct sockaddr_in);
	bzero(&saddr, len);

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(2430);
	saddr.sin_addr.s_addr = inet_addr("192.168.1.20");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		printf("create sock fail!\n");
		return -1;
	}
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	ret = sendto(sockfd, w_buf, 50, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if(ret < 0){
		printf("send error!\n");
		return -1;
	}


	close(sockfd);
	return 0;
}
