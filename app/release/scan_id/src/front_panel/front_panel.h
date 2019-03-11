#ifndef __FRONT_PANEL_H
#define __FRONT_PANEL_H

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

#include "../include/other_gpio.h"
#include "../include/i2c-dev.h"
#include "../main.h"
#include "./oled.h"
#include "./led.h"

/* 最大显示页面 */
#define MAXPAGE		5

/* 最大灯显示颜色 */
#define MAXCOLOUR	0x04

/* 显示板从地址 */
#define PANEL_ADDR  0x50

/* 寄存器地址 */
#define KEY		0x00	//按键数据		
#define CLEAN	0x01	//清屏
#define HEART   0x02	//心跳信号
#define S_CHAR	0x03	//更新单个字符
#define S_LED	0x04	//更新单个LED
#define A_LED	0x05	//更新全部LED
#define FIRST	0x06	//更新第一行
#define SECOND  0x07	//更新第二行
#define THIRD	0x08	//更新第三行
#define FOURTH	0x09	//更新第四行

/* LED 颜色定义 */
#define CLOSE	0x00	//关闭
#define RED		0x01	//红色
#define GREEN	0x02	//绿色
#define BLUE	0x03	//蓝色
#define YELLOW	0x04	//黄色
#define PINK	0x05	//粉色	
#define CYAN	0x06	//青色
#define WHITE	0x07	//白色

/* 按键值定义 */
#define	UP		0x0F
#define	SET		0x1F
#define	CH1		0x2F
#define	CH2		0x3F
#define	CH3		0x4F
#define	CH4		0x5F
#define	CH5		0x6F
#define	CH6		0x7F
//#define			0x8F
//#define			0x9F
//#define			0xF0
//#define			0xF1
#define	CH12	0xF2
#define	CH11	0xF3
#define	CH10	0xF4
#define	CH9		0xF5
#define	CH8		0xF6
#define	CH7		0xF7
#define	ENTER	0xF8
#define	DOWN	0xF9

void front_panel();

#endif
