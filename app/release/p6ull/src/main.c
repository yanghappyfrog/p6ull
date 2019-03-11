/*************************************************************************
    > File Name: p437x.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 11时02分31秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include "./main.h"

#define LOG_FILE "/root/pavt/log.txt"

#define BUFSZ 32
#define PORT 6464
#define IP "127.0.0.1"


int write_log(char *str)
{
	FILE *log_filep;
	time_t timer;
	struct tm *tblock;
	
	timer = time(NULL);
	tblock = localtime(&timer);

	log_filep = fopen(LOG_FILE, "a+");
	if(log_filep == NULL){
		printf("open log file error!\n");
		return -1;
	}

	fprintf(log_filep, "%s", asctime(tblock));
	fprintf(log_filep, "%s\n\n", str);

	fclose(log_filep);
}

int main(void)
{
	int ret = 0, sin_len;
	pthread_t pth_key;
	pthread_t pth_spi;
	pthread_t pth_scan;
	pthread_t pth_date;

	bzero(&gval.address, sizeof(gval.address));  
	gval.address.sin_family = AF_INET;
	gval.address.sin_port = htons(PORT);
	gval.address.sin_addr.s_addr = inet_addr(IP);
	sin_len = sizeof(gval.address);

	gval.clifd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == gval.clifd){
		perror("socket error!\n");
		return -1;
	}
	//bind(clifd, (struct sockaddr *)&address, sizeof(address));

	gval.buz_fd = open(BUZZER_DEV, O_RDWR, 0);
	if(gval.buz_fd < 0){
		perror("open buzzer error\n");
		return -1;
	}

	/* 获取版本号并更新文件 /root/pavt/info/version.info */
	get_version();

	/*
	 * 在目录 /sys/class/pwm/pwmchip0 下
	 * 创建控制风扇设备节点
	 */
	//ret = fan_dev_create();
	//if(ret != 0 ){
	//	printf("fan error!\n");
	//	write_log("fan_error!");
	//}

	/*
	 * 读取面板按键值，并发送给应用层app
	 */
	if(pthread_create(&pth_key, NULL, send_key_val, NULL) == -1){
		printf("fail to create pthread key!\n");
		write_log("fail to create pthread key!");
	}

	/*
	 * 向STM32发送心跳信号，心跳异常STM32会复位主芯片
	 */
	//if(pthread_create(&pth_spi, NULL, send_spi_heart, NULL) == -1){
	//	printf("fail to create pthread spi!\n");
	//	write_log("fail to create pthread spi!");
	//}

	/*
	 * 扫描扩展串口，确定各扩展口外接的何种设备并获取数据,并存到文件中
	 */
	//if(pthread_create(&pth_scan, NULL, scan_com, NULL) == -1){
	//	printf("fail to create pthread scan_com!\n");
	//	write_log("fail to create pthread scan_com!");
	//}

	//pthread_join(pth_spi, NULL);
	//pthread_join(pth_scan, NULL);
	pthread_join(pth_key, NULL);

	return 0;
}


