/*************************************************************************
    > File Name: spi.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 14时40分29秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "./spi.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev1.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

//static void transfer(int fd, int mes)
static void transfer(int fd, int val)
{
	int ret, i;
	
	uint8_t tx[8] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};


	if(val == 1){
		for(i = 0; i < 8; i++)
			tx[i] = 0x1;
	}else if(val == 3){
		for(i = 0; i < 8; i++)
			tx[i] = 0x8;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	printf("----------- send data ---------------\n");
	for(i = 0; i < 8; i++)
		printf("send[%d]: 0x%x\n", i, tx[i]);

//	printf("----------- recv data ---------------\n");
//	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
//		if (!(ret % 6))
//			puts("");
//		printf("%.2X ", rx[ret]);
//	}
//	puts("");
//	printf("-------------------------------------\n");
}


//int main(int argc, char *argv[])
void *send_spi_heart(void *a)
{
	int ret = 0, i = 0;
	int fd;
	int mes;

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	//printf("spi mode: %d\n", mode);
	//printf("bits per word: %d\n", bits);
	//printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	//mes = atoi(argv[1]);
	//printf("mes: 0x%x\n", mes);
	//transfer(fd, mes);
	transfer(fd, 1); //start
	while(1)
	{
		transfer(fd, 2); //heart data
		sleep(10);
	}

	//transfer(fd, 3); //end
	close(fd);

	return ret;
}


