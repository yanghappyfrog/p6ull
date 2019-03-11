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

#define P6ULL_FILE "/root/pavt/ini/pavt.ini"
#define EX_UART_FILE "/root/pavt/info/ex_uart.info"
#define CIR_DATA_FILE "/root/pavt/info/cir_data.info"
#define ELEC_DATA_FILE "/root/pavt/info/elec_data.info"
#define GPS_DATA_FILE "/root/pavt/info/gps_data.info"
#define POWER_DATA_FILE "/root/pavt/info/power_data.info"
#define IDDATA_FILE "/backup/pavt/info/id_data.info"

struct data {
	int cir;
	int elec;
	int gps;
	int power;
	int id;
};


int update_data(void);
