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

#define P6ULL_FILE				"/root/pavt/ini/p6ull.ini"
#define IDDATA_FILE				"/backup/pavt/info/id_data.info"

#define EX_UART_FILE			"/root/pavt/info/ex_uart.info"
#define CIR_DATA_FILE			"/root/pavt/info/cir_data.info"
#define ELEC_DATA_FILE			"/root/pavt/info/elec_data.info"
#define GPS_DATA_FILE			"/root/pavt/info/gps_data.info"
#define POWER_DATA_FILE			"/root/pavt/info/power_data.info"
#define AIRSWITCH_DATA_FILE		"/root/pavt/info/airswitch_data.info"
#define TMP_DATA_FILE			"/root/pavt/info/tmp_data.info"

#define UART_DEV "/dev/ttymxc1"
#define UART_GPIO "/dev/uart_gpio0"

struct data {
	int cir;
	int elec;
	int gps;
	int power;
	int id;
};


int update_data(void);
