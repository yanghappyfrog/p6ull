/*************************************************************************
    > File Name: pavt_error.h
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年01月24日 星期三 17时55分22秒
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <termio.h>
#include <time.h>
#include <jni.h>

#include "com_zjpavt_client_nativemethod_JnIMethod.h"
#include "serial.h"
#include "other_gpio.h"
#include "i2c-dev.h"
#include "dictionary.h"
#include "iniparser.h"

#define KEYPAD_DEV "/dev/i2c-0"
#define UART_DEV "/dev/ttymxc1"
#define EX_CARD "/dev/other_gpio0"
#define UART_GPIO "/dev/uart_gpio0"
#define I2C_ADDR 0x50

#define CHANNEL 4
#define ELECT	"ELECT"			//电流电压检测模块
#define POWER	"POWER"			//远程抄表模块
#define GPS_LUX "GPS"			//GPS光感模块
#define CIRCUIT "CIRCUIT"		//回路控制器
#define ID		"ID"			//扫码器
#define SWITCH	"SWITCH"		//开关门报警模块
#define AIR		"AIR"			//自动重合闸
#define TMP		"TMP"			//温湿度控制器
#define NUL		"NUL"

#define OK				0
#define OLED_ERR		1
#define LED_ERR			2
#define BUZ_ERR			3
#define POW_GET_ERR		4
#define GPS_GET_ERR		5
#define LIG_GET_ERR		6
#define CH_SET_ERR		7
#define CIR_GET_ERR		8

//#define DEBUG

#if defined(DEBUG)
#define jni_dbg(format, arg...)		\
	printf(format, ##arg)
#else
#define jni_dbg(format, arg...)				\
({								\
	if (0)							\
		printf(format, ##arg);	\
})
#endif


