/*************************************************************************
    > File Name: main.h
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年05月08日 星期二 11时00分31秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "./fan/pwmconfig.h"
#include "./key/other_gpio.h"
#include "./spi/spi.h"
#include "./scan_com/scan_com.h"
#include "./get_data/get_data.h"
#include "./version/version.h"

#define BUZZER_DEV "/dev/other_gpio0"

struct globe {
	struct sockaddr_in address;  
	int clifd;
	int buz_fd;
};
struct globe gval;

