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

/* show board result */
void get_page3_data()
{
	dictionary *ini;
	char *str;
	char key[16] = {0};

	clean_oled();
	usleep(500*1000);
	ini = iniparser_load(TEST_RESULT_FILE);

	bzero(gval.oled, sizeof(gval.oled));
	bzero(key, sizeof(key));
	sprintf(key, "board:temp");
	str = iniparser_getstring(ini, key, "null");
	sprintf(gval.oled[0], "*temp:%s", str);

	bzero(key, sizeof(key));
	sprintf(key, "board:rtc");
	str = iniparser_getstring(ini, key, "null");
	sprintf(gval.oled[1], "*rtc:%s", str);
	
	bzero(key, sizeof(key));
	sprintf(key, "board:net");
	str = iniparser_getstring(ini, key, "null");
	sprintf(gval.oled[2], "*net:%s", str);

	bzero(key, sizeof(key));
	sprintf(key, "board:4g");
	str = iniparser_getstring(ini, key, "null");
	sprintf(gval.oled[3], "*4g:%s", str);

	flash_oled(gval.oled);
	iniparser_freedict(ini);
}

/* show uart result */
void get_page4_data()
{
	int i;
	dictionary *ini;
	char *str;
	char key[16] = {0};

	clean_oled();
	usleep(500*1000);
	ini = iniparser_load(TEST_RESULT_FILE);

	bzero(gval.oled, sizeof(gval.oled));
	for(i = 0; i < 4; i++)
	{
		bzero(key, sizeof(key));
		sprintf(key, "uart:uart%d", i);
		str = iniparser_getstring(ini, key, "null");
		sprintf(gval.oled[i], "*uart%d:%s", i, str);
	}

	flash_oled(gval.oled);
	iniparser_freedict(ini);
}
void get_page5_data()
{
	dictionary *ini;
	char *str;
	char key[16] = {0};

	clean_oled();
	usleep(500*1000);
	ini = iniparser_load(TEST_RESULT_FILE);

	bzero(gval.oled, sizeof(gval.oled));
	bzero(key, sizeof(key));
	sprintf(key, "uart:uart4");
	str = iniparser_getstring(ini, key, "null");
	sprintf(gval.oled[0], "*uart4:%s", str);

	flash_oled(gval.oled);
}

/* show gps lun info */
void get_page6_data()
{
	
}
