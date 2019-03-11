/*************************************************************************
    > File Name: oled.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月11日 星期三 13时48分21秒
 ************************************************************************/

#include "./oled.h"

/*
 * 更新指定字符
 * @x: 列号
 * @y: 行号
 * @c: 字符
 */
int flash_char(int x, int y, char c)
{
	int ret;
	char w_buf[3] = {0};

	if(x < 0 | x > 15){
		perror("oled x error\n");
		return -1;
	}
	if(y < 0 | y > 3){
		perror("oled y error\n");
		return -1;
	}

	w_buf[0] = x;
	w_buf[1] = y;
	w_buf[2] = c;
	
	i2c_smbus_write_i2c_block_data(gdev.panel_fd, S_CHAR, sizeof(w_buf), w_buf);

	i2c_smbus_read_byte_data(gdev.panel_fd, 0x02);
	return 0;
}

/*
 * 更新一行数据
 * @w_buf: 内容
 * @line: 号行
 */
int flash_line(char *w_buf, int line)
{
	switch(line)
	{
		case 0:
			i2c_smbus_write_i2c_block_data(gdev.panel_fd, FIRST, strlen(w_buf), w_buf);	
			break;
		case 1:
			i2c_smbus_write_i2c_block_data(gdev.panel_fd, SECOND, strlen(w_buf), w_buf);	
			break;
		case 2:
			i2c_smbus_write_i2c_block_data(gdev.panel_fd, THIRD, strlen(w_buf), w_buf);	
			break;
		case 3:
			i2c_smbus_write_i2c_block_data(gdev.panel_fd, FOURTH, strlen(w_buf), w_buf);	
			break;
		default:
			perror("line error\n");
			return -1;
	}
	
	i2c_smbus_read_byte_data(gdev.panel_fd, 0x02);

	return 0; 
}

/* 清屏 */
void clean_oled()
{
	int ret;
	 
	while(1)
	{
		i2c_smbus_read_byte_data(gdev.panel_fd, 0x1);
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			break;
	}
	usleep(15*1000);
}
/*
 * 更新整屏数据
 */
int flash_oled(char w_buf[4][16])
{
	int ret;

	while(1)
	{
		i2c_smbus_read_byte_data(gdev.panel_fd, 0x1);
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		i2c_smbus_write_i2c_block_data(gdev.panel_fd, FIRST, sizeof(w_buf[0]), w_buf[0]);	
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
		break;
	}
	usleep(15*1000);
	
	while(1)
	{
		i2c_smbus_write_i2c_block_data(gdev.panel_fd, SECOND, sizeof(w_buf[1]), w_buf[1]);	
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		i2c_smbus_write_i2c_block_data(gdev.panel_fd, THIRD, sizeof(w_buf[2]), w_buf[2]);	
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		i2c_smbus_write_i2c_block_data(gdev.panel_fd, FOURTH, sizeof(w_buf[3]), w_buf[3]);	
		ret = i2c_smbus_read_byte_data(gdev.panel_fd, 0x1F);
		if(ret == 1)
			break;
	}

	i2c_smbus_read_byte_data(gdev.panel_fd, 0x02);
	return 0;
}


void oled_init()
{
	printf("*** oled_init ***\n");
	strcpy(gval.oled[0], "***  PAVT  *****");
	strcpy(gval.oled[1], "*  P6ULL TEST  *");
	strcpy(gval.oled[2], "*   PROGRAM    *");
	strcpy(gval.oled[3], "*  version 1.0 *");
	flash_oled(gval.oled);

	//strcpy(line, "** version 1.0 *");
	//flash_line(line, 4);

	return ;
}
