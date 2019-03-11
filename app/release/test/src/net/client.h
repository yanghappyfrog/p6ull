/*************************************************************************
    > File Name: client.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月16日 星期一 15时57分33秒
 ************************************************************************/

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>  
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#define BUFSZ 32
#define PORT 6464
#define IP "192.168.1.101"
//#define IP "127.0.0.1"

#include "../main.h"

int net_service();
