/*************************************************************************
    > File Name: p6ull_test.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年09月28日 星期三 14时02分31秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include "./main.h"

int data_init()
{

	bzero(&gdev, sizeof(gdev));
	bzero(&gval, sizeof(gval));

	gdev.clifd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == gdev.clifd){
		perror("socket error!\n");
		return -1;
	}

	gdev.mix_fd = open(MIX_DEV, O_RDWR, 0);
	if(gdev.mix_fd < 0){
		perror("open other_gpio0 error\n");
		return -1;
	}

	gdev.panel_fd = open(FRONT_PANEL, O_RDWR, 0);
	if(gdev.panel_fd < 0){
		perror("open i2c-0 error\n");
		return -1;
	}
	if(ioctl(gdev.panel_fd, I2C_SLAVE_FORCE, PANEL_ADDR)){
		perror("set panel slave addr error!\n");
		return -1;
	}

	gdev.key_fd = open(KEY_DEV, O_RDWR, 0);
	if(gdev.panel_fd < 0){
		perror("open keypad0 error\n");
		return -1;
	}

}

int check_id()
{
	dictionary *ini;
	char *id;

	ini = iniparser_load(ID_FILE);
	
	id = iniparser_getstring(ini, "id_data:id", "null");
	printf("id: %s\n", id);

	return 0;
}

int main(void)
{
	int ret = 0;

	data_init();

	printf("\n");
	printf("\n");
	printf("******************************************************\n");
	printf("**********   PAVT SCAN ID PROGRAM  *******************\n");
	printf("******************************************************\n");
	printf("\n");
	printf("\n");

	ret = check_id();		//检查id_data.txt文件中的id是否正确
	if(ret != 0){
		scan_com();
	}

	close(gdev.clifd);
	close(gdev.exuart_fd);
	close(gdev.exuart_addr);
	close(gdev.inuart_fd);
	close(gdev.mix_fd);
	close(gdev.panel_fd);
	close(gdev.key_fd);
	printf("********** test program end! *******************\n");
	return 0;
}


