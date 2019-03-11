/*************************************************************************
    > File Name: scan_com.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 16时57分06秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
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

#include "../iniparser/iniparser.h"
#include "../iniparser/dictionary.h"
#include "./scan_com.h"
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
#define ID		"ID"
#define SWITCH	"SWITCH"
#define NUL		"NUL"

/*
 * 解析返回数据,并保存到ini文件中
 * 1:elect; 2:power; 3:gps_lux; 4:circuit; 5:id; 6:switch; 7:nul
 *
 */
int analyze_buf(dictionary *ini, char *r_buf, int channel)
{
	int ret;
	char key[12] ={0};
	char *p = NULL;
	char temp[512] = {0};

	//printf("*** %s *** \n", __func__);
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		goto ERR;
	}
	 
	ret = strncmp(r_buf, "<IDDATA_OK>", strlen("<IDDATA_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		printf("idddata get ok[%s]!!\n", key);
		iniparser_set(ini, key, ID);
		return 0;
	}
	ret = strncmp(r_buf, "<ELECT_OK>", strlen("<ELECT_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		printf("elect get ok[%s]!!\n",  key);
		iniparser_set(ini, key, ELECT);
		return 0;
	}
	ret = strncmp(r_buf, "<POWER_OK>", strlen("<POWER_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		printf("POWER get ok[%s]!!\n",  key);
		iniparser_set(ini, key, POWER);
		return 0;
	}
	ret = strncmp(r_buf, "<CIRCUIT_ON/OFF_CH", strlen("<CIRCUIT_ON/OFF_CH"));			//CH后面为回路数，待扩展
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		p = strstr(r_buf, "CH");
		r_buf = p;
		p = strchr(r_buf, '_');
		*p = '\0';
		sprintf(temp, "CIRCUIT_%s", r_buf);
		printf("CIRCUIT get ok[%s][%s]!!\n", key, temp);
		iniparser_set(ini, key, temp);
		return 0;
	}
	ret = strncmp(r_buf, "<LUX_GPS_DATA_OK>", strlen("<LUX_GPS_DATA_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		printf("GPS get ok[%s]!!\n", key);
		iniparser_set(ini, key, GPS_LUX);
		return 0;
	}
	ret = strncmp(r_buf, "<SWITCH_DATA_OK>", strlen("<SWITCH_DATA_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		printf("SWITCH get ok[%s]!!\n", key);
		iniparser_set(ini, key, SWITCH);
		return 0;
	}


ERR:
	printf("data error!!\n");

	return -1;
}


int scan_exuart()
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char key[12] ={0};

	char * ini_name = "/root/pavt/info/ex_uart.info";

	FILE *ex_uart_filep;
    dictionary * ini ;
	
	printf("*** %s *** \n", __func__);
    ini = iniparser_load(ini_name);
    //iniparser_dump(ini, stdout);

	/* scan exuart */
	for(i = 1; i < CHANNEL + 1; i++)
	{
		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "<AT_CHECK%d>", i);					//发送握手指令:<AT_CHECKi>
		printf("scan_com: ex%d send data: %s\n", i, w_buf);
		rs485_write((1 << (i - 1)), w_buf);		
		bzero(r_buf, sizeof(r_buf));
		ret = rs485_read((1 << (i - 1)), r_buf);
		if(ret < 0){
			sprintf(key, "extend_uart:ex%d", i);
			printf(" ex%d no device !!\n", i);
			iniparser_set(ini, key, NUL);
			continue;
		}
		printf("ex%d recv data: %s\n", i, r_buf);

		analyze_buf(ini, r_buf, i);
		sleep(1); 
	}

	ex_uart_filep = fopen("/root/pavt/info/ex_uart.info", "w");
	if(ex_uart_filep == NULL){
		printf("open file error!\n");
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, ex_uart_filep);

	iniparser_freedict(ini);
	fclose(ex_uart_filep);
	return 0;
}

void *scan_com(void *a)
{
	printf("*** scan_com pthread ***\n");
	while(1)
	{
		//scan_exuart();	//扫描扩展串口接入的何种设备
		update_data();	//从接入设备获取数据
		sleep(1);
	}

	return 0;
}
