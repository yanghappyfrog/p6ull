/*************************************************************************
    > File Name: get_data.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月02日 星期五 10时15分13秒
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

#include "../main.h"
#include "../iniparser/iniparser.h"
#include "./get_data.h"
#include "../serial/serial.h"

#define P6ULL_FILE "/root/pavt/ini/p6ull.ini"
#define EX_UART_FILE "/root/pavt/info/ex_uart.info"
#define CIR_DATA_FILE "/root/pavt/info/cir_data.info"
#define ELEC_DATA_FILE "/root/pavt/info/elec_data.info"
#define GPS_DATA_FILE "/root/pavt/info/gps_data.info"
#define POWER_DATA_FILE "/root/pavt/info/power_data.info"
#define ID_DATA_FILE "/root/pavt/info/id_data.info"

#define UART_DEV "/dev/ttymxc1"
#define UART_GPIO "/dev/uart_gpio0"

struct data {
	int cir;
	int elec;
	int gps;
	int power;
	int id;
};

#if 0
int get_channle(struct data *ch_data)
{
	FILE *ini_file;
	char line[32];
	char *p;
	//char ch[2];
	
	ini_file = fopen(EX_UART_FILE, "r");
	if(ini_file == NULL){
		printf("open ini file error!\n");
		return -1;
	}
	
	//get elect ch
	while(fgets(line, sizeof(line), ini_file))
	{
		if(strstr(line, "ELECT")){
			p = strchr(line, '=');
			*p = '\0';
			p = strchr(line, 'x');
			//sprintf(ch, "%s", p+1);
			ch_data->elec = atoi(p+1);
			printf("elect line: %s, ch: %s, elec_ch: %d\n", line, p+1, ch_data->elec);
			break;
		}
	}
	rewind(ini_file);

	//get power ch
	while(fgets(line, sizeof(line), ini_file))
	{
		if(strstr(line, "POWER")){
			p = strchr(line, '=');
			*p = '\0';
			p = strchr(line, 'x');
			//sprintf(ch, "%s", p+1);
			ch_data->power = atoi(p+1);
			printf("power line: %s, ch: %s, power_ch: %d\n", line, p+1, ch_data->power);
			break;
		}
	}
	rewind(ini_file);
	
	//get cir ch
	while(fgets(line, sizeof(line), ini_file))
	{
		if(strstr(line, "CIRCUIT")){
			p = strchr(line, '=');
			*p = '\0';
			p = strchr(line, 'x');
			//sprintf(ch, "%s", p+1);
			ch_data->cir = atoi(p+1);
			printf("circuit line: %s, ch: %s, power_ch: %d\n", line, p+1, ch_data->cir);
			break;
		}
	}
	rewind(ini_file);

	//get gps ch
	while(fgets(line, sizeof(line), ini_file))
	{
		if(strstr(line, "GPS")){
			p = strchr(line, '=');
			*p = '\0';
			p = strchr(line, 'x');
			//sprintf(ch, "%s", p+1);
			ch_data->gps = atoi(p+1);
			printf("gps line: %s, ch: %s, power_ch: %d\n", line, p+1, ch_data->gps);
			break;
		}
	}
	rewind(ini_file);

	//get id ch
	while(fgets(line, sizeof(line), ini_file))
	{
		if(strstr(line, "ID")){
			p = strchr(line, '=');
			*p = '\0';
			p = strchr(line, 'x');
			//sprintf(ch, "%s", p+1);
			ch_data->id = atoi(p+1);
			printf("id line: %s, ch: %s, power_ch: %d\n", line, p+1, ch_data->id);
			//break;
			return 0;
		}
	}
	rewind(ini_file);
	
	printf("not found!\n");
	fclose(ini_file);
	return -1;
}
#endif

int get_temp_data()
{
	int ret, temp_fd, temp_int;
	float temp_float;
	char temp_buf[10] = {0};
	char key[10] = {0};
	char buf[10] = {0};

	FILE *filep;
	dictionary *ini;

	printf("### %s ###\n", __func__);
	temp_fd = open("/sys/class/hwmon/hwmon1/temp1_input", O_RDONLY);
	if(temp_fd < 0){
		printf("open temp device error!\n");
		close(temp_fd);
		return -1;
	}

	ret = read(temp_fd, temp_buf, 10);
	if(ret < 0){
		printf("read temp error!\n");
		close(temp_fd);
		return -1;
	}
	if(read == 0){
		printf("Nothing can be read now!\n");
		close(temp_fd);
		return -1;
	}
	temp_float = atoi(temp_buf)/1000;
	printf("Now tempreture is: %f\n", temp_float);

	sprintf(buf, "%f", temp_float);
    ini = iniparser_load(GPS_DATA_FILE);
    //iniparser_dump(ini, stdout);
	sprintf(key, "gps_data:temp");
	iniparser_set(ini, key, buf);

	filep = fopen(GPS_DATA_FILE, "w");
	if(filep == NULL){
		printf("open file error!\n");
		close(temp_fd);
		fclose(filep);
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, filep);

	fclose(filep);
    iniparser_freedict(ini);
	close(temp_fd);
	return 0;
}

int get_switch_data(int sw_ch)
{
	int ret;
	char key[12] ={0};
	char w_buf[32] = {0};
	char r_buf[32] = {0};

	FILE *filep;
	dictionary *ini;

	printf("### %s ###\n", __func__);
	ini = iniparser_load(P6ULL_FILE);

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_DOOR>", sw_ch);	//发送查询回路驱动信息
	rs485_write(sw_ch, w_buf);
	printf("get_switch_data: ex%d send data: %s\n", sw_ch, w_buf);
	ret = rs485_read(sw_ch, r_buf);
	if(ret < 0) 
		return -1;
	
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "ERROR")){
		printf("data error!\n");
		return -1;
	}

	ret = strncmp(r_buf, "<DOOR_alarm>", strlen("<DOOR_alarm>"));
	if(ret == 0){
		//printf("door open!\n");
		sprintf(r_buf, "%s\n", r_buf);
		sendto(gval.clifd, &r_buf, sizeof(r_buf), 0, (struct sockaddr *)&gval.address, sizeof(gval.address));		
		ioctl(gval.buz_fd, IOREQ_BUZZER_ON);
		sprintf(key, "door:status");
		iniparser_set(ini, key, "open");
	}
	ret = strncmp(r_buf, "<DOOR_close>", strlen("<DOOR_close>"));
	if(ret == 0){
		//printf("door close!\n");
		sprintf(r_buf, "%s\n", r_buf);
		sendto(gval.clifd, &r_buf, sizeof(r_buf), 0, (struct sockaddr *)&gval.address, sizeof(gval.address));		
		ioctl(gval.buz_fd, IOREQ_BUZZER_OFF);
		sprintf(key, "door:status");
		iniparser_set(ini, key, "close");
	}

	filep = fopen(P6ULL_FILE, "w");
	if(filep == NULL){
		printf("open file error!\n");
		fclose(filep);
		iniparser_freedict(ini);
		return -1;
	}
	iniparser_dump_ini(ini, filep);

	fclose(filep);
	iniparser_freedict(ini);
	return 0;
}

int get_cir_data(int cir_ch)
{
	int i, ret = 0, ch = 0;
	char *p;
	char key[24] = {0};
	char w_buf[128] = {0};
	char r_buf[512] = {0};
	//char r_buf[512] = "<CIRCUIT_CURRENT_DATA_CH12_[A_231.0][B_232.0][C_233.0][001/49.01/1][002/49.02/1][003/49.03/1][004/49.04/1][005/49.05/1][006/49.06/1][007/49.07/1][008/49.08/1][009/49.09/1][010/49.10/1][011/49.11/1][012/49.12/1]>";
	char temp[512] = {0};
	char vals[16][32] = {0} ;

	FILE *cir_filep;
    dictionary * ini ;

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_CIRCUIT_CURRENT_DATA>", cir_ch);	//发送查询回路驱动信息


	/* cir_ch == 0 从内部串口uart3获取数据 */
	if(cir_ch == 0){
		rs232_write(w_buf);
		printf("get_cir_data: send data: %s\n", w_buf);
	} else {
		rs485_write(cir_ch, w_buf);
		printf("get_cir_data: ex%d send data: %s\n", cir_ch, w_buf);
	}

	if(cir_ch == 0){
		rs232_read(r_buf);
		printf("recv data: %s\n", r_buf);
	} else {
		rs485_read(cir_ch, r_buf);
		printf("recv data: %s\n", r_buf);
	}
	strcpy(temp, r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "ERROR")){
		printf("data error!\n");
		return -1;
	}
	ret = strncmp(r_buf, "<CIRCUIT_CURRENT_DATA", strlen("<CIRCUIT_CURRENT_DATA"));
	if(ret != 0){
		printf("data error!\n");
		return -1;
	}

	/*获取通道数*/
	p = strstr(temp, "CH");
	sprintf(temp, "%s", p);
	p = strchr(temp, '_');
	*p = '\0';
	p = strchr(temp, 'H');
	ch = atoi(p+1);
	printf("ch: %d\n", ch);

	/* 获取通道电流信息 */
	for(i = ch; i > 0; i--)
	{
		p = strrchr(r_buf, ']');
		*p = '\0';
		p = strrchr(r_buf, '[');
		bzero(temp, sizeof(temp));
		sprintf(temp, p+1);

		p = strchr(temp, '/');
		strncpy(vals[i], p+1, strlen(p+1));
		*p = '\0';
	}

	/* 获取A B C三相电压 */
	for(i = ch + 3; i > ch; i--)
	{
		p = strrchr(r_buf, ']');
		*p = '\0';
		p = strrchr(r_buf, '[');
		bzero(temp, sizeof(temp));
		sprintf(temp, p+1);
		
		p = strchr(temp, '_');
		strncpy(vals[i], p+1, strlen(p+1));
		//printf("p: %s\n", p+1);
		*p = '\0';
	}
	//for(i = 1; i < ch+4; i++)
	//	printf("vals[%d]: %s\n", i, vals[i]);

	/* 将数据保存到文件中 */
    ini = iniparser_load(CIR_DATA_FILE);
    //iniparser_dump(ini, stdout);

	for(i = 1; i < ch+1; i++)
	{
		sprintf(key, "ex%d_cir_data:ch%d", cir_ch, i);
		iniparser_set(ini, key, vals[i]);
	}

	sprintf(key, "ex%d_cir_data:a", cir_ch);
	iniparser_set(ini, key, vals[ch+1]);
	sprintf(key, "ex%d_cir_data:b", cir_ch);
	iniparser_set(ini, key, vals[ch+2]);
	sprintf(key, "ex%d_cir_data:c", cir_ch);
	iniparser_set(ini, key, vals[ch+3]);

	cir_filep = fopen(CIR_DATA_FILE, "w");
	if(cir_filep == NULL){
		printf("open file error!\n");
		fclose(cir_filep);
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, cir_filep);

	fclose(cir_filep);
    iniparser_freedict(ini);
	return 0;
}

int get_elec_data(int elec_ch)
{
	int i, ret = 0;

	char key[12] ={0};
	char *p;
	char channel[2] = {0};
	//char w_buf[128] = {0};
	char w_buf[512] = "<AT_CHECK0_ELECT_[A001_001][A002_002][A003_003][B004_004][B005_005][B006_006][C007_007][C008_008][C009_009][A010_010][A011_011][A012_012][B013_013][B014_014][B015_015][C016_016]>";
	char r_buf[512] = {0};
	//char r_buf[512] = "<ELECT_[001_A/220/00.0][002_A/220/00.0][003_A/220/00.0][004_A/220/00.0][005_A/220/00.0][006_A/220/00.0][007_A/220/00.0][008_A/220/00.0][009_A/220/00.0][010_A/220/00.0][011_A/220/00.0][012_A/220/00.0][013_A/220/00.0][014_A/220/00.0][015_A/220/00.0][016_A/220/00.0]>";
	char vals[16][32] = {0} ;

	FILE *elec_filep;
	FILE *fp;
    dictionary * ini ;
    char       * ini_name = ELEC_DATA_FILE;

	time_t timep;

	//bzero(w_buf, sizeof(w_buf));
	//sprintf(w_buf, "<AT_CHECK%d_ELECT_DATA>", elec_ch);	//发送查询回路驱动信息
	rs485_write(elec_ch, w_buf);
	printf("write data: %s\n", w_buf);
	rs485_read(elec_ch, r_buf);
	printf("recv data: %s\n", r_buf);

	fp = fopen("/root/test/elect.log", "a+");
	if(fp == NULL){
		printf("open file error!\n");
		return -1;
	}

	time(&timep);
	fprintf(fp, "%s", asctime(gmtime(&timep)));
	fprintf(fp, "%s\n", r_buf);


#if 0
	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		return -1;
	}
	 
	for(i = 15; i >=0; i--)
	{
		p = strrchr(r_buf, ']');
		*p = '\0';
		p = strrchr(r_buf, '[');
		strncpy(vals[i], p+1, strlen(p+1));
		*p = '\0';
	}
	for(i = 0; i < 16; i++)
		printf("vals[%d]: %s\n", i, vals[i]);

    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);

	for(i = 0; i < 16; i++)
	{
		sprintf(key, "elec_data:ch%d", i);
		iniparser_set(ini, key, vals[i]);
	}

	elec_filep = fopen(ELEC_DATA_FILE, "w");
	if(elec_filep == NULL){
		printf("open file error!\n");
		return -1;
	}
    iniparser_dump_ini(ini, elec_filep);

	fclose(elec_filep);
    iniparser_freedict(ini);
#endif

	fclose(fp);
	return 0;
}

int get_gps_data(int gps_ch)
{
	int i = 0, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char temp[128] = {0};
	char temp_buf[128] = {0};
	char key[24] = {0};
	char data[20][20] = {0} ;
	char *p = NULL;
	char *p_temp = NULL;

	FILE *gps_filep;
    dictionary * ini ;

	printf("### %s ###\n", __func__);
	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_LUX_GPS_DATA>", gps_ch);	//发送查询光照GPS指令
	rs485_write(gps_ch, w_buf);
	printf("get_gps_data: ex%d send data: %s\n", gps_ch, w_buf);
	rs485_read(gps_ch, r_buf);
	printf("recv data: %s\n", r_buf);
	strcpy(temp, r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "ERROR")){
		printf("data error!\n");
		return -1;
	}
	ret = strncmp(r_buf, "<LUX_GPS_DATA", strlen("<LUX_GPS_DATA"));
	if(ret != 0){
		printf("data error!\n");
		return -1;
	}


	/* 获取光照数据 */
	p = strrchr(temp, '/');
	*p = '\0';
	//printf("**************\n");
	p = strrchr(temp, '_');
	//printf("p: %s\n", p);
	strncpy(data[0], p+1, strlen(p+1));
	//printf("data[%d]:%s\n", 0, data[0]);	//LUX: data[0]

	p = strrchr(r_buf, '/');
	strcpy(temp, p+1);
	//printf("temp: %s\n", temp);	

	//if(strncmp(temp, "$GNGGA", 6)){
	if(strncmp(temp, "$GNRMC", 6)){
		printf("gps data error!\n");
		return -1;
	}

    ini = iniparser_load(GPS_DATA_FILE);
    //iniparser_dump(ini, stdout);
	sprintf(key, "gps_data:lux");
	iniparser_set(ini, key, data[0]);

	gps_filep = fopen(GPS_DATA_FILE, "w");
	if(gps_filep == NULL){
		printf("open file error!\n");
		fclose(gps_filep);
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, gps_filep);

	fclose(gps_filep);
    iniparser_freedict(ini);

	/* 获取gps信息
	 * data[1]: UTC时间，格式:hhmmss.sss
	 * data[2]: 定位有效标识: V:无效; A:有效
	 * data[3]: 纬度
	 * data[5]: 经度
	 * data[9]: UTC日期，格式:ddmmyy
	 * 其他值可忽略
	 */
	i = 1;
	while(1)
	{
		//printf("**************\n");
		p = strchr(temp, ',');
		if(p == NULL) break;
		sprintf(temp_buf, "%s", p+1);
		sprintf(temp, "%s", p+1);
		//printf("temp_buf: %s\n", temp_buf);
		p_temp = temp_buf;

		p = strchr(temp_buf, ',');
		if(p == NULL) break;
		*p = '\0';
		strcpy(data[i], temp_buf);
		printf("data[%d]: %s\n", i, data[i]);
		//printf("temp_buf: %s\n", p_temp);

		i++;
	}


    ini = iniparser_load(GPS_DATA_FILE);
    //iniparser_dump(ini, stdout);

	sprintf(key, "gps_data:gps_lat");
	iniparser_set(ini, key, data[3]);
	sprintf(key, "gps_data:gps_lng");
	iniparser_set(ini, key, data[5]);

	gps_filep = fopen(GPS_DATA_FILE, "w");
	if(gps_filep == NULL){
		printf("open file error!\n");
		fclose(gps_filep);
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, gps_filep);

	fclose(gps_filep);
    iniparser_freedict(ini);
	return 0;
}

int get_power_data(int uart_fd, int gpio_fd, int power_ch)
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	//char r_buf[128] = "<POWER_123456.0_[A/220/000.0][B/220/000.0][C/220/000.0]>";
	char vals[4][32] = {0} ;
	char *p = NULL;
	char key[24] = {0};
	
	FILE *power_filep;
    dictionary * ini ;
    char       * ini_name = POWER_DATA_FILE;

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_POWER>", power_ch);	//发送查询电能表指令
	rs485_write(power_ch, w_buf);
	printf("write data: %s\n", w_buf);
	rs485_read(power_ch, r_buf);
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		return -1;
	}

	//获取三相电电流电压:vals[0]~vals[2]
	for(i = 0; i < 3; i++)
	{
		p = strrchr(r_buf, ']');
		*p = '\0';
		p = strrchr(r_buf, '[');
		strncpy(vals[i], p+1, strlen(p+1));
		*p = '\0';
		printf("vals[%d]: %s\n", i, vals[i]);
	}

	//获取功率:vals[3]
	p = strrchr(r_buf, '_');
	*p = '\0';
	p = strrchr(r_buf, '_');
	strncpy(vals[3], p+1, strlen(p+1));
	printf("vals[3]: %s\n", vals[3]);

    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);

	sprintf(key, "power_data:power");
	iniparser_set(ini, key, vals[3]);
	sprintf(key, "power_data:a");
	iniparser_set(ini, key, vals[2]);
	sprintf(key, "power_data:b");
	iniparser_set(ini, key, vals[1]);
	sprintf(key, "power_data:c");
	iniparser_set(ini, key, vals[0]);

	power_filep = fopen(POWER_DATA_FILE, "w");
	if(power_filep == NULL){
		printf("open file error!\n");
		return -1;
	}
    iniparser_dump_ini(ini, power_filep);

	fclose(power_filep);
    iniparser_freedict(ini);
	return 0;
}

int get_id_data(int uart_fd, int gpio_fd, int id_ch)
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	//char r_buf[128] = "<IDDATA_AAAAAAAAAAAAAAAAAAA>";
	char key[24] = {0};
	char vals[1][32] = {0} ;
	char *p = NULL;

	FILE *id_filep;
    dictionary *ini;
    char       *ini_name = ID_DATA_FILE;

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_IDDATA>", id_ch);	//发送查询ID指令
	rs485_write(id_ch, w_buf);
	printf("write data: %s\n", w_buf);
	rs485_read(id_ch, w_buf);
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		return -1;
	}

	p = strrchr(r_buf, '>');
	*p = '\0';							//除去最后的'>'

	/* 获取数据 */
	p = strrchr(r_buf, '_');
	strncpy(vals[0], p+1, strlen(p+1));
	*p = '\0';
	printf("vals:%s\n", vals[0]);		

    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);

	sprintf(key, "id_data:id");
	iniparser_set(ini, key, vals[0]);

	id_filep = fopen(ID_DATA_FILE, "w");
	if(id_filep == NULL){
		printf("open file error!\n");
		return -1;
	}
    iniparser_dump_ini(ini, id_filep);

	fclose(id_filep);
    iniparser_freedict(ini);
	return 0;
}

/* 获取各通道对应的设备 
 * 1:elect; 2:power; 3:gps_lux; 4:circuit; 5:id
 */
int get_channle()
{
	int channel, ret, i;
    dictionary * ini ;
	char *str;
	char key[16] = {0};
	
	get_cir_data(0);		//get in_uart data
	//get_temp_data();

    ini = iniparser_load(EX_UART_FILE);
    //iniparser_dump(ini, stdout);

	for(i = 1; i < 5; i++)
	{
		sprintf(key, "extend_uart:ex%d", i);
		str = iniparser_getstring(ini, key, "null");

		channel = (1 << (i - 1)); //扩展通道硬件地址为:1,2,4,8
		printf("channel: %d; dev: %s\n", channel, str);

		ret = strncmp(str, "NUL", strlen("NUL"));
		if(ret == 0){
			printf("ex%d no device !\n", i);
			continue;
		}

		ret = strncmp(str, "SWITCH", strlen("SWITCH"));
		if(ret == 0){
			printf("ex%d device is SWITCH!\n", channel);
			get_switch_data(channel);
			continue;
		}

		ret = strncmp(str, "GPS", strlen("GPS"));
		if(ret == 0){
			printf("ex%d device is GPS_LUX!\n", channel);
			get_gps_data(channel);
			continue;
		}

		ret = strncmp(str, "CIRCUIT", strlen("CIRCUIT"));
		if(ret == 0){
			printf("ex%d device is CIRCUIT!\n", channel);
			get_cir_data(channel);
			continue;
		}

		ret = strncmp(str, "ELECT", strlen("ELECT"));
		if(ret == 0){
			printf("ex%d device is ELECT!\n", channel);
			get_elec_data(channel);
			continue;
		}
	}

    iniparser_freedict(ini);
}

int update_data(void)
{
	struct data channel_data = {0};
	int ret;

	printf("**************** update_data ********************\n");
	get_channle();			//获取各通道对应的设备并读取数据

	return 0;
}
 

