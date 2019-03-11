/*************************************************************************
    > File Name: scan_exuart.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年06月28日 星期四 14时35分57秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <time.h>

#include "./iniparser.h"
#include "./serial.h"

#define CHANNEL 4

#define ELECT	"ELECT"		//电流电压检测模块
#define POWER	"POWER"		//远程抄表模块
#define GPS_LUX "GPS"		//GPS光感模块
#define CIRCUIT "CIRCUIT"	//回路控制器
#define ID		"ID"		//扫码器
#define SWITCH	"SWITCH"	//开关门报警模块
#define AIR		"AIR"		//自动重合闸
#define TMP		"TMP"		//温湿度控制器
#define NUL		"NUL"

int scan_exuart();

