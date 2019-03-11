/*************************************************************************
    > File Name: scan_com.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 16时57分06秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/

#include "./scan_com.h"

int get_id_data(int id_ch)
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	//char r_buf[128] = "<IDDATA_AAAAAAAAAAAAAAAAAAA>";
	char key[24] = {0};
	char vals[64] = {0} ;
	char *p = NULL;

	FILE *id_filep;
    dictionary *ini;

    ini = iniparser_load(ID_FILE);
    iniparser_dump(ini, stdout);

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_IDDATA>", id_ch);	//发送查询ID指令
	rs485_write(id_ch, w_buf);
	printf("id write data: %s\n", w_buf);
	rs485_read(id_ch, r_buf);
	printf("recv id data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		perror(" id data error!\n");
		return -1;
	}

	p = strrchr(r_buf, '>');
	*p = '\0';							//除去最后的'>'

	/* 获取数据 */
	p = strrchr(r_buf, '_');
	strncpy(vals, p+1, strlen(p+1));
	*p = '\0';
	printf("vals:%s\n", vals);		

	sprintf(key, "id_data:id");
	iniparser_set(ini, key, vals);

	id_filep = fopen(ID_FILE, "w");
	if(id_filep == NULL){
		perror("open file error!\n");
		fclose(id_filep);
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, id_filep);

	fclose(id_filep);
    iniparser_freedict(ini);

	system("/bin/cp /backup/pavt/info/id_data.info /root/pavt/info/id_data.info");
	return 0;
}


int scan_exuart()
{
	int i = 0;
	int ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char key[12] ={0};

	char *p = NULL;
	char temp[512] = {0};

	FILE *ex_uart_filep;
    dictionary * ini ;
	
	printf("*** %s *** \n", __func__);
    //iniparser_dump(ini, stdout);

	/* scan ex_uart */
	for(i = 1; i < CHANNEL + 1; i++)
	{
		bzero(w_buf, sizeof(w_buf));
		sprintf(w_buf, "<AT_CHECK%d>", i);					//发送握手指令:<AT_CHECKi>
		printf("scan_com: ex%d send data: %s\n", i, w_buf);
		rs485_write((1 << (i - 1)), w_buf);		
		bzero(r_buf, sizeof(r_buf));
		ret = rs485_read((1 << (i - 1)), r_buf);
		if(ret < 0){
			sprintf(key, "uart:uart%d", i);
			printf(" ex%d no device !!\n", i);
			continue;
		}
		printf("ex%d recv data: %s\n", i, r_buf);

		ret = strncmp(r_buf, "<SCANQRCODE_DATA_OK>", strlen("<SCANQRCODE_DATA_OK>"));
		if(ret == 0){
			//sprintf(key, "extend_uart:ex%d", channel);
			sprintf(key, "uart:uart%d", i);
			printf("scanqrcode get ok[%s]!!\n", key);
		}

		/* get id */
		get_id_data(i);

		sleep(1); 
	}

	return 0;
}

void scan_com()
{
	int i = 0;


	//while(1)
	for(i = 0; i < 3; i++)
	{
		printf("********** scan ex_com **********\n");
		scan_exuart();	//扫描扩展串口接入的何种设备
		//update_data();	//从接入设备获取数据
		sleep(1);
	}

	printf("********** scan ex_com  over! **********\n");
	return ;
}
