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

#include "../main.h"
#include "../iniparser/iniparser.h"
#include "../iniparser/dictionary.h"
#include "../serial/serial.h"
#include "../get_data/get_data.h"

#define UART_DEV "/dev/ttymxc1"
#define UART_GPIO "/dev/uart_gpio0"
#define CHANNEL 4

//#define ELECT	"1"
//#define POWER	"2"
//#define GPS_LUX "3"
//#define CIRCUIT "4"
//#define ID		"5"
#define ELECT	"ELECT"
#define POWER	"POWER"
#define GPS_LUX "GPS"
#define CIRCUIT "CIRCUIT"
#define SCANER	"SCANER"
#define SWITCH	"SWITCH"
#define NUL		"NUL"

void *scan_com(void *);

