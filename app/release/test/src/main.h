/*************************************************************************
    > File Name: main.h
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年05月08日 星期二 11时00分31秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/

#ifndef __MAIN_H__
#define __MAIN_H__ 

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "./include/i2c-dev.h"
#include "./pool/pool.h"
#include "./front_panel/front_panel.h"
#include "./board/board.h"
#include "./key/other_gpio.h"
#include "./spi/spi.h"
#include "./scan_com/scan_com.h"
#include "./get_data/get_data.h"
#include "./version/version.h"
#include "./net/client.h"

#define MIX_DEV		"/dev/other_gpio0"			//for buzzer, extend card and so on. 
#define E_UART		"/dev/ttymxc1"				//uart2  extend uart
#define E_UART_ADDR	"/dev/uart_gpio0"			//for select which extend uart
#define I_UART		"/dev/ttymxc2"				//uart3  inside uart
#define FRONT_PANEL	"/dev/i2c-0"				//
#define KEY_DEV		"/dev/keypad0"				//for key

#define TEST_RESULT_FILE "/home/root/test_result.txt"

#define POOL_RUN_NUM  4

struct pavt_dev {
	struct sockaddr_in address;  
	int clifd;
	int exuart_fd;
	int exuart_addr;
	int inuart_fd;
	int mix_fd;
	int panel_fd;
	int key_fd;
};
struct pavt_dev gdev;

struct pavt_val {
	int key_val;
	int page;
	char led[16];
	char oled[4][16];
};
struct pavt_val gval;


#endif
