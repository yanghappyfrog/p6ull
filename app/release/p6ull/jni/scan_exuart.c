/*************************************************************************
    > File Name: scan_exuart.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年06月28日 星期四 14时35分53秒
 ************************************************************************/

#include "./pavt_jni.h"
#include "./scan_exuart.h"

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
		perror("<jni> scan recv data error!\n");
		goto ERR;
	}
	 
	//ret = strncmp(r_buf, "<SCANQRCODE_DATA_OK>", strlen("<SCANQRCODE_DATA_OK>"));
	//if(ret == 0){
	//	sprintf(key, "extend_uart:ex%d", channel);
	//	//printf("idddata get ok[%s]!!\n", key);
	//	iniparser_set(ini, key, ID);
	//	return 0;
	//}
	
	//ret = strncmp(r_buf, "<ELECT_OK>", strlen("<ELECT_OK>"));
	//if(ret == 0){
	//	sprintf(key, "extend_uart:ex%d", channel);
	//	//printf("elect get ok[%s]!!\n",  key);
	//	iniparser_set(ini, key, ELECT);
	//	return 0;
	//}
	
	ret = strncmp(r_buf, "<POWER_OK>", strlen("<POWER_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		//printf("POWER get ok[%s]!!\n",  key);
		iniparser_set(ini, key, POWER);
		return 0;
	}

	ret = strncmp(r_buf, "<CIRCUIT_ON/OFF_CH", strlen("<CIRCUIT_ON/OFF_CH"));			//CH后面为回路数
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		p = strstr(r_buf, "CH");
		r_buf = p;
		p = strchr(r_buf, '_');
		*p = '\0';
		sprintf(temp, "CIRCUIT_%s", r_buf);
		//printf("CIRCUIT get ok[%s][%s]!!\n", key, temp);
		iniparser_set(ini, key, temp);
		return 0;
	}

	ret = strncmp(r_buf, "<LUX_GPS_DATA_OK>", strlen("<LUX_GPS_DATA_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		//printf("GPS get ok[%s]!!\n", key);
		iniparser_set(ini, key, GPS_LUX);
		return 0;
	}

	ret = strncmp(r_buf, "<SWITCH_DATA_OK>", strlen("<SWITCH_DATA_OK>"));
	if(ret == 0){
		sprintf(key, "extend_uart:ex%d", channel);
		//printf("SWITCH get ok[%s]!!\n", key);
		iniparser_set(ini, key, SWITCH);
		return 0;
	}

ERR:
	perror("<jni> scan recv data error!\n");
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

	system("cp /backup/pavt/info/* /root/pavt/info/");  //清空原有文件内容

	ini = iniparser_load(ini_name);
    //iniparser_dump(ini, stdout);

	/* scan exuart */
	/* 从第二个扩展口开始扫描,第一个口默认接电流电压检测模块 */
	for(i = 2; i < CHANNEL + 1; i++)
	{
		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "<AT_CHECK%d>", i);					//发送握手指令:<AT_CHECKi>
		jni_dbg("scan_com: ex%d send data: %s\n", i, w_buf);
		rs485_write((1 << (i - 1)), w_buf);		
		bzero(r_buf, sizeof(r_buf));
		ret = rs485_read((1 << (i - 1)), r_buf);
		if(ret < 0){
			sprintf(key, "extend_uart:ex%d", i);
			printf(" ex%d no device !!\n", i);
			iniparser_set(ini, key, NUL);
			continue;
		}
		jni_dbg("ex%d recv data: %s\n", i, r_buf);

		analyze_buf(ini, r_buf, i);
		sleep(1); 
	}

	ex_uart_filep = fopen("/root/pavt/info/ex_uart.info", "w");
	if(ex_uart_filep == NULL){
		perror("open file error!\n");
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, ex_uart_filep);

	iniparser_freedict(ini);
	fclose(ex_uart_filep);
	return 0;
}

