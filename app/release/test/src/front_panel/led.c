/*************************************************************************
    > File Name: led.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月11日 星期三 10时26分34秒
 ************************************************************************/

#include "./led.h"

/*
 * 修改指定LED颜色
 * @ x: 列号(0~8)
 * @ y: 行号(0~1)
 * @ c: 颜色值
 */
int flash_led(int x, int y, int c)
{
	int ret;
	char w_buf[3] = {0};

	if(y < 0 | y > 1){
		perror("led y error!\n");
		return -1;
	}

	if(x < 0 | x > 8){
		perror("led x error!\n");
		return -1;
	}

	w_buf[0] = x;
	w_buf[1] = y;
	w_buf[2] = c;

	while(1)
	{
		i2c_smbus_write_i2c_block_data(gdev.panel_fd, S_LED, sizeof(w_buf), w_buf);	
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			return 0;
	}
	return -1;
}


/*
 * 修改单个通道状态
 * @num: 通道号
 * @status:状态值
 * @ch:设备所在的扩展通道
 */
int flash_ch(int num, int statu, int ch)
//int flash_ch(int num, int ch)
{
	int ret;

	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char s_num[2] = {0};

	num += 1;
	if(num < 10)
		sprintf(s_num, "0%d", num);
	else
		sprintf(s_num, "%d", num);

	//sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_ON_%s%d>", ch, s_num, gval.led[num - 1]);
	sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_ONE_%s%d>", ch, s_num, statu - 1);
	printf("### %s: w_buf: %s\n", __func__, w_buf);

	if(ch == 0)
		rs232_write(w_buf);
	else
		rs485_write((1 << ch), w_buf);

	return 0;	
}

int flash_all_ch()
{

	return 0;
}

/* 
 * 修改所有led灯颜色 
 * @ *w_buf: 16位灯的颜色值
 */
//int flash_all_led(char *w_buf)
int flash_all_led()
{
	int i, ret;
	char led_status[16] = {0};

	//for(i = 0; i < strlen(w_buf); i++)
	//	led_status[i] = w_buf[i];

	while(1)
	{
		//i2c_smbus_write_i2c_block_data(gdev.panel_fd, A_LED, sizeof(led_status), led_status);	
		i2c_smbus_write_i2c_block_data(gdev.panel_fd, A_LED, sizeof(gval.led), gval.led);	
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			return 0;
	}

	return -1;
}

void change_ch_led()
{
	if(gval.key_val == 0x2f){			//CH1
		gval.led[0] += 1;
		if(gval.led[0] > MAXCOLOUR)
			gval.led[0] = 1;
		flash_ch(0, gval.led[0], 0);
	}
	if(gval.key_val == 0x3f){			//CH2
		gval.led[1] += 1;
		if(gval.led[1] > MAXCOLOUR)
			gval.led[1] = 1;
		flash_ch(1, gval.led[1], 0);
	}

	if(gval.key_val == 0x4f){			//CH3
		gval.led[2] += 1;
		if(gval.led[2] > MAXCOLOUR)
			gval.led[2] = 1;
		flash_ch(2, gval.led[2], 0);
	}

	if(gval.key_val == 0x5f){			//CH4
		gval.led[3] += 1;
		if(gval.led[3] > MAXCOLOUR)
			gval.led[3] = 1;
		flash_ch(3, gval.led[3], 0);
	}

	if(gval.key_val == 0x6f){			//CH5
		gval.led[4] += 1;
		if(gval.led[4] > MAXCOLOUR)
			gval.led[4] = 1;
		flash_ch(4, gval.led[4], 0);
	}

	if(gval.key_val == 0x7f){			//CH6
		gval.led[5] += 1;
		if(gval.led[5] > MAXCOLOUR)
			gval.led[5] = 1;
		flash_ch(5, gval.led[5], 0);
	}

	if(gval.key_val == 0xf7){			//CH7
		gval.led[6] += 1;
		if(gval.led[6] > MAXCOLOUR)
			gval.led[6] = 1;
		flash_ch(6, gval.led[6], 0);
	}

	if(gval.key_val == 0xf6){			//CH8
		gval.led[7] += 1;
		if(gval.led[7] > MAXCOLOUR)
			gval.led[7] = 1;
		flash_ch(7, gval.led[7], 0);
	}

	if(gval.key_val == 0xf5){			//CH9
		gval.led[8] += 1;
		if(gval.led[8] > MAXCOLOUR)
			gval.led[8] = 1;
		flash_ch(8, gval.led[8], 0);
	}

	if(gval.key_val == 0xf4){			//CH10
		gval.led[9] += 1;
		if(gval.led[9] > MAXCOLOUR)
			gval.led[9] = 1;
		flash_ch(9, gval.led[9], 0);
	}

	if(gval.key_val == 0xf3){			//CH11
		gval.led[10] += 1;
		if(gval.led[10] > MAXCOLOUR)
			gval.led[10] = 1;
		flash_ch(10, gval.led[10], 0);
	}

	if(gval.key_val == 0xf2){			//CH12
		gval.led[11] += 1;
		if(gval.led[11] > MAXCOLOUR)
			gval.led[11] = 1;
		flash_ch(11, gval.led[11], 0);
	}


	//flash_all_ch();
	flash_all_led();
	return;
}

int led_init()
{
	int i, ret;
	char w_buf[16] = {0};

	printf("*** led_init ***\n");

//	memset(gval.led, RED, sizeof(gval.led));
//	ret = flash_all_led(gval.led);
//	sleep(1);
//
//	memset(gval.led, GREEN, sizeof(gval.led));
//	ret = flash_all_led(gval.led);
//	sleep(1);

	memset(gval.led, BLUE, sizeof(gval.led));
	ret = flash_all_led(gval.led);
	sleep(1);

//	flash_led(0, 0, 4);

	printf("*** led_init ok! ***\n");
	return 0;
}
