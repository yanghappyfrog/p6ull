/*************************************************************************
    > File Name: scan_com.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 16时57分06秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/

#include "./scan_com.h"

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
		//sprintf(key, "extend_uart:ex%d", channel);
		sprintf(key, "uart:uart%d", channel);
		printf("idddata get ok[%s]!!\n", key);
		iniparser_set(ini, key, SCANER);
		return 0;
	}
	ret = strncmp(r_buf, "<ELECT_OK>", strlen("<ELECT_OK>"));
	if(ret == 0){
		//sprintf(key, "extend_uart:ex%d", channel);
		sprintf(key, "uart:uart%d", channel);
		printf("elect get ok[%s]!!\n",  key);
		iniparser_set(ini, key, ELECT);
		return 0;
	}
	ret = strncmp(r_buf, "<POWER_OK>", strlen("<POWER_OK>"));
	if(ret == 0){
		//sprintf(key, "extend_uart:ex%d", channel);
		sprintf(key, "uart:uart%d", channel);
		printf("POWER get ok[%s]!!\n",  key);
		iniparser_set(ini, key, POWER);
		return 0;
	}
	ret = strncmp(r_buf, "<CIRCUIT_ON/OFF_CH", strlen("<CIRCUIT_ON/OFF_CH"));			//CH后面为回路数，待扩展
	if(ret == 0){
		//sprintf(key, "extend_uart:ex%d", channel);
		sprintf(key, "uart:uart%d", channel);
		p = strstr(r_buf, "CH");
		r_buf = p;
		p = strchr(r_buf, '_');
		*p = '\0';
		sprintf(temp, "CIR_%s", r_buf);
		printf("CIRCUIT get ok[%s][%s]!!\n", key, temp);
		iniparser_set(ini, key, temp);
		return 0;
	}
	ret = strncmp(r_buf, "<LUX_GPS_DATA_OK>", strlen("<LUX_GPS_DATA_OK>"));
	if(ret == 0){
		//sprintf(key, "extend_uart:ex%d", channel);
		sprintf(key, "uart:uart%d", channel);
		printf("GPS get ok[%s]!!\n", key);
		iniparser_set(ini, key, GPS_LUX);
		return 0;
	}
	ret = strncmp(r_buf, "<SWITCH_DATA_OK>", strlen("<SWITCH_DATA_OK>"));
	if(ret == 0){
		//sprintf(key, "extend_uart:ex%d", channel);
		sprintf(key, "uart:uart%d", channel);
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
	int i = 0;
	int ret;
	char w_buf[512] = {0};
	char r_buf[512] = {0};
	char key[12] ={0};

	char *p = NULL;
	char temp[512] = {0};

	FILE *ex_uart_filep;
    dictionary * ini ;
	
	printf("*** %s *** \n", __func__);
    ini = iniparser_load(TEST_RESULT_FILE);
    //iniparser_dump(ini, stdout);

	/* scan in_uart */
	sprintf(w_buf, "<AT_CHECK%d>", i);					//发送握手指令:<AT_CHECKi>
	printf("scan_com: ex%d send data: %s\n", i, w_buf);
	rs232_write(w_buf);
	usleep(10*1000);
	bzero(r_buf, sizeof(r_buf));
	ret = rs232_read(r_buf);
	if(ret < 0){
		sprintf(key, "uart:uart%d", i);
		printf(" ex%d no device !!\n", i);
		iniparser_set(ini, key, NUL);
	}else{
		printf("ex%d recv data: %s\n", i, r_buf);
		sprintf(key, "uart:uart%d", i);
		p = strstr(r_buf, "CH");
		//r_buf = p;
		strcpy(r_buf, p);
		p = strchr(r_buf, '_');
		*p = '\0';
		sprintf(temp, "CIR_%s", r_buf);
		printf("CIRCUIT get ok[%s][%s]!!\n", key, temp);
		iniparser_set(ini, key, temp);
	}

	/* scan ex_uart */
	for(i = 1; i < CHANNEL + 1; i++)
	{
		bzero(w_buf, sizeof(w_buf));
		//sprintf(w_buf, "<AT_CHECK%d>", i);					//发送握手指令:<AT_CHECKi>
		sprintf(w_buf, "<AT_CHECK%d_ELECT_[A001_001][A002_002][A003_003][A004_004][A005_005][B006_006][B007_007][C008_008][C009_009][B010_010][C011_011][B012_012]>", i);  	//发送握手指令:<AT_CHECKi>
		printf("scan_com: ex%d send data: %s\n", i, w_buf);
		rs485_write((1 << (i - 1)), w_buf);		
		bzero(r_buf, sizeof(r_buf));
		ret = rs485_read((1 << (i - 1)), r_buf);
		if(ret < 0){
			sprintf(key, "uart:uart%d", i);
			printf(" ex%d no device !!\n", i);
			iniparser_set(ini, key, NUL);
			continue;
		}
		printf("ex%d recv data: %s\n", i, r_buf);

		analyze_buf(ini, r_buf, i);
		sleep(1); 
	}

	ex_uart_filep = fopen(TEST_RESULT_FILE, "w");
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
	int i = 0;

	//system("cp /home/root/test_result.bak /home/root/test_result.txt");

	//while(1)
	for(i = 0; i < 3; i++)
	{
		printf("********** scan_com pthread **********\n");
		scan_exuart();	//扫描扩展串口接入的何种设备
		//update_data();	//从接入设备获取数据
		sleep(1);
	}

	printf("********** scan_com pthread over! **********\n");
	return ;
}
