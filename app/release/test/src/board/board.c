/*************************************************************************
    > File Name: board.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月17日 星期二 10时35分04秒
 ************************************************************************/

#include "./board.h"

void detect_temp(dictionary *ini)
{
	int ret, temp_fd, temp_int;
	float temp_float;
	char temp_buf[10] = {0};
	char key[10] = {0};
	char buf[10] = {0};

	printf("### %s ###\n", __func__);
	sprintf(key, "board:temp");

	if(access("/sys/class/hwmon/hwmon1", F_OK) == -1){
		printf("temp error!\n");
		goto temp_err;
	}

	temp_fd = open("/sys/class/hwmon/hwmon0/temp1_input", O_RDONLY);
	if(temp_fd < 0){
		printf("open temp device error!\n");
		goto temp_err;
	}

	ret = read(temp_fd, temp_buf, 10);
	if(ret < 0){
		printf("read temp error!\n");
		goto temp_err;
	}
	temp_float = atoi(temp_buf)/1000;
	printf("Now tempreture is: %f\n", temp_float);
	sprintf(buf, "%f", temp_float);
	iniparser_set(ini, key, buf);

	close(temp_fd);
	return;

temp_err:
	iniparser_set(ini, key, "NUL");
	close(temp_fd);
	return;
}

void detect_clock(dictionary *ini)
{
	int ret;
	char key[10] = {0};

	printf("### %s ###\n", __func__);
	sprintf(key, "board:rtc");

	if(access("/sys/class/rtc/rtc0", F_OK) != -1){
		printf("rtc dev OK!\n");
		iniparser_set(ini, key, "OK");
	}else{
		printf("rtc dev error!\n");
		iniparser_set(ini, key, "NUL");
	}

	return;
}

void detect_net(dictionary *ini)
{
	int ret;
	char key[10] = {0};

	printf("### %s ###\n", __func__);
	sprintf(key, "board:net");
	
	if((access("/sys/class/net/eth0", F_OK) == -1) \
		| (access("/sys/class/net/eth1", F_OK)== -1)){
		printf("eth dev error!\n");
		iniparser_set(ini, key, "NUL");
	}else{
		printf("eth dev OK!\n");
		iniparser_set(ini, key, "OK");
	}

	return;
}

void detect_4g(dictionary *ini)
{
	int ret;
	char key[10] = {0};

	printf("### %s ###\n", __func__);
	sprintf(key, "board:4g");

	if(access("/sys/bus/usb-serial/devices/ttyUSB0", F_OK) != -1){
		printf("4g dev OK!\n");
		iniparser_set(ini, key, "OK");
	}else{
		printf("4g dev error!\n");
		iniparser_set(ini, key, "NUL");
	}

	return;
}

void detect_board(void)
{
	FILE *board_filep;
	dictionary *ini;

	//system("cp /home/root/test_result.bak /home/root/test_result.txt");
	
	printf("*********** detect_board pthread **************\n");
	ini = iniparser_load(TEST_RESULT_FILE);

	detect_temp(ini);
	detect_clock(ini);
	detect_net(ini);
	detect_4g(ini);

	board_filep = fopen(TEST_RESULT_FILE, "w");
	if(board_filep == NULL){
		printf("open file error!\n");
		iniparser_freedict(ini);
		return ;
	}
	iniparser_dump_ini(ini, board_filep);

	iniparser_freedict(ini);
	fclose(board_filep);
	printf("*********** detect_board pthread over! **************\n");
	return;
}

