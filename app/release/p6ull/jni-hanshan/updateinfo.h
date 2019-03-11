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

#define P6ULL_FILE				"/root/pavt/ini/pavt.ini"
#define EX_UART_FILE			"/root/pavt/info/ex_uart.info"

/* 
 *  使用电流电压检测模块，
 *  将数据存入回路控制器文件中，
 *  回路控制器中的电流电压数据暂时未使用 
 */
//#define CIR_DATA_FILE			"/root/pavt/info/cir_data.info"
//#define ELEC_DATA_FILE		"/root/pavt/info/elec_data.info"
/* just for hanshan */
#define CIR_DATA_FILE			"/root/pavt/info/elec_data.info"
#define ELEC_DATA_FILE			"/root/pavt/info/cir_data.info"

#define AIRSWITCH_DATA_FILE 	"/root/pavt/info/airswitch_data.info"
#define GPS_DATA_FILE			"/root/pavt/info/gps_data.info"
#define POWER_DATA_FILE			"/root/pavt/info/power_data.info"
#define IDDATA_FILE				"/backup/pavt/info/id_data.info"

struct data {
	int cir;
	int elec;
	int gps;
	int power;
	int id;
};


int update_data(void);
