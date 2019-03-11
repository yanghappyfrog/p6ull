/*************************************************************************
    > File Name: front_panel.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月11日 星期三 09时48分36秒
 ************************************************************************/

#include "./front_panel.h"

void show_page()
{
	switch(gval.page)
	{
		case 0:
			break;
		case 1:
			get_page1_data();
			break;
		case 2:
			get_page2_data();
			break;
		case 3:
			get_page3_data();
			break;
		case 4:
			get_page4_data();
			break;
		case 5:
			get_page5_data();
			break;
	}

	return;
}

void show_last_page()
{
	gval.page -= 1;
	if(gval.page < 1)
		gval.page = MAXPAGE;

	printf("show page: %d\n", gval.page);
	show_page();

}

void show_next_page()
{
	gval.page += 1;
	if(gval.page > MAXPAGE)
		gval.page = 1;

	printf("show page: %d\n", gval.page);
	show_page();
}

void analy_key(int key_val)
{
	printf("page val: %d\n", gval.page);
	switch(key_val)
	{
		case UP:
			show_last_page();
			break;
		case DOWN:
			show_next_page();
			break;

		case CH1:
		case CH2:
		case CH3:
		case CH4:
		case CH5:
		case CH6:
		case CH7:
		case CH8:
		case CH9:
		case CH10:
		case CH11:
		case CH12:
			change_ch_led();
			break;

		//case SET:
		//	show_set(page);
		//case ENTER:
		//	show_enter(page);
		
		default: 
			printf("the key val error!\n");
			return ;
	}

	return;
}

void init_panel()
{

	oled_init();
	led_init();

	return;
}

void read_key(int num)
{
	ioctl(gdev.mix_fd, IOREQ_BUZZER_ON);
	read(gdev.key_fd, &gval.key_val, sizeof(gval.key_val));
	printf("### key_val: 0x%x\n", gval.key_val);
	usleep(50*1000);
	ioctl(gdev.mix_fd, IOREQ_BUZZER_OFF);

	analy_key(gval.key_val);

}

void front_panel(void)
{
	int oflags;

	init_panel();
	signal(SIGIO, read_key);
	fcntl(gdev.key_fd, F_SETOWN, getpid());
	oflags = fcntl(gdev.key_fd, F_GETFL);
	fcntl(gdev.key_fd, F_SETFL, oflags | FASYNC);
	
	while(1)
	{
		printf("*********** front_panel pthread *************\n");
		sleep(10);
	}

	return; 
}
