/*************************************************************************
    > File Name: p6ull_test.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年07月11日 星期三 09时02分31秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include "./main.h"

#define LOG_FILE "/root/pavt/log.txt"

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

int main(void)
{
	int ret = 0;

	data_init();

	system("cp /home/root/test_result.bak /home/root/test_result.txt");

	pool_init(POOL_RUN_NUM);
	printf("\n");
	printf("\n");
	printf("******************************************************\n");
	printf("********** PAVT P6ULL TEST PROGRAM *******************\n");
	printf("******************************************************\n");
	printf("\n");
	printf("\n");

	/* 获取版本号并更新文件 /root/pavt/info/version.info */
	//pool_add_worker(get_version, NULL);

	/* 风扇测试 */
	//pool_add_worker(fan_dev_create, NULL);

	/* 板载芯片测试 */
	pool_add_worker(detect_board, NULL);
	sleep(2);

	/* 显示面板测试 */
	pool_add_worker(front_panel, NULL);
	sleep(2);

	/* 串口测试 */
	pool_add_worker(scan_com, NULL);

	/* tcp/ip server */
	//pool_add_worker(net_service, NULL);

	pool_destroy();
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


