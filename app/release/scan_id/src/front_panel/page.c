/*************************************************************************
    > File Name: page.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年07月11日 星期三 10时26分20秒
 ************************************************************************/

#include "./page.h"

void get_page1_data()
{
	bzero(gval.oled, sizeof(gval.oled));
	strcpy(gval.oled[0], "****************");
	strcpy(gval.oled[1], "*  P6ULL TEST  *");
	strcpy(gval.oled[2], "*   PROGRAM    *");
	strcpy(gval.oled[3], "****************");
	flash_oled(gval.oled);
}

/* show time */
void get_page2_data()
{
	int i = 0;
	struct tm *t;
	time_t tt;
	struct sysinfo info;

	printf("### show page2 ###\n");
	bzero(gval.oled, sizeof(gval.oled));
	time(&tt);
	sysinfo(&info);
	t = localtime(&tt);
	printf("data:%4d-%02d-%02d, week:%d, time:%02d:%02d:%02d, boot_time: %ld\n", \
			t->tm_year+1900, t->tm_mon + 1, t->tm_mday, \
			t->tm_wday, t->tm_hour, t->tm_min, t->tm_sec, \
			info.uptime);

	sprintf(gval.oled[0], "TIME %02d:%02d:%02d   ", t->tm_hour, t->tm_min, t->tm_sec);
	sprintf(gval.oled[1], "WEEK %d          ", t->tm_wday);
	sprintf(gval.oled[2], "%04d-%02d-%02d       ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
	sprintf(gval.oled[3], "%ld    %d", info.uptime, i);

	flash_oled(gval.oled);

	while(1)
	{
		time(&tt);
		sysinfo(&info);
		t = localtime(&tt);

		sprintf(gval.oled[0], "TIME %02d:%02d:%02d   ", t->tm_hour, t->tm_min, t->tm_sec);
		flash_line(gval.oled[0], 0);
		usleep(20*1000);
		sprintf(gval.oled[3], "%ld    %d", info.uptime, i);
		flash_line(gval.oled[3], 3);

		usleep(100*1000);
		i++;
		if(gval.page != 2)
			break;
	}

	return ;
}

/* show id */
void get_page3_data()
{
	dictionary *ini;
	char *str;
	char key[16] = {0};

	clean_oled();
	usleep(500*1000);
	ini = iniparser_load();


}

