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

#define SCANER	"SCANER"

void scan_com();

