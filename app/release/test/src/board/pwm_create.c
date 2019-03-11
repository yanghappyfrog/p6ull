/*************************************************************************
    > File Name: pwm_create.c
    > Author: Happyfrog
    > Mail: ygb@zjpavt.com 
    > Created Time: 2018年03月01日 星期四 10时59分12秒
    > Copyright(c) PAVT CO.,LTD. All rights reserved. 
 ************************************************************************/
#include <stdio.h>
#include "./pwmconfig.h"

/* PWM export */
int pwm_export(unsigned int pwm)
{
	int fd;

	switch(pwm){
	case 1:{
		fd = open(SYSFS_PWM_DIR "/pwmchip0/export", O_WRONLY);
		if (fd < 0) {
			printf ("\nFailed export PWM_1\n");
			return -1;
		}

		write(fd, "0", 2);
		close(fd);
		return 0;
		break;

	}
	case 2:{
		fd = open(SYSFS_PWM_DIR "/pwmchip0/export", O_WRONLY);
		if (fd < 0) {
			printf ("\nFailed export PWM_2\n");
			return -1;
		}

		write(fd, "1", 2);
		close(fd);
		return 0;
		break;
	}
	default:{
		printf("wrong PWM channel input\n");
		return -1;
	}
	}

}

/* PWM unexport */
int pwm_unexport(unsigned int pwm)
{
	int fd;

	switch(pwm){
	case 1:{
		fd = open(SYSFS_PWM_DIR "/pwmchip0/export", O_WRONLY);
		if (fd < 0) {
			printf ("\nFailed unexport PWM_1\n");
			return -1;
		}

		write(fd, "0", 2);
		close(fd);
		return 0;
        break;
	}
	case 2:{
		fd = open(SYSFS_PWM_DIR "/pwmchip0/export", O_WRONLY);
		if (fd < 0) {
			printf ("\nFailed unexport PWM_2\n");
			return -1;
		}

		write(fd, "1", 2);
		close(fd);
		return 0;
		break;
	}
	default:{
		printf("wrong PWM channel input\n");
		return -1;
	}
	}
}

/* PWM configuration */
int pwm_config(unsigned int pwm, unsigned int period, unsigned int duty_cycle)
{
	int fd,len_p,len_d;
	char buf_p[MAX_BUF];
	char buf_d[MAX_BUF];

	len_p = snprintf(buf_p, sizeof(buf_p), "%d", period);
	len_d = snprintf(buf_d, sizeof(buf_d), "%d", duty_cycle);

	switch(pwm){
	case 1:{
			/* set pwm period */
		    fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm0/period", O_WRONLY);
			if (fd < 0) {
				printf ("\nFailed set PWM_1 period\n");
				return -1;
			}

			write(fd, buf_p, len_p);
			/* set pwm duty cycle */
			fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm0/duty_cycle", O_WRONLY);
			if (fd < 0) {
				printf ("\nFailed set PWM_1 duty cycle\n");
				return -1;
			}

			write(fd, buf_d, len_d);

			close(fd);
			return 0;
			break;
		}
		case 2:{
			/* set pwm period */
			fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm1/period", O_WRONLY);
			if (fd < 0) {
				printf ("\nFailed set PWM_2 period\n");
				return -1;
			}

			write(fd, buf_p, len_p);
			/* set pwm duty cycle */
			fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm1/duty_cycle", O_WRONLY);
			if (fd < 0) {
				printf ("\nFailed set PWM_2 duty cycle\n");
				return -1;
			}

			write(fd, buf_d, len_d);

			close(fd);
			return 0;
			break;
		}
		default:{
			printf("wrong PWM channel input\n");
			return -1;
		}
		}
}

/* PWM enable */
int pwm_enable(unsigned int pwm)
{
	int fd;

	switch(pwm){
	case 1:{
		    fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm0/enable", O_WRONLY);
			if (fd < 0) {
				printf ("\nFailed enable PWM_1\n");
				return -1;
			}
			write(fd, "1", 2);

			close(fd);
			return 0;
			break;
		}
		case 2:{
			fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm1/enable", O_WRONLY);
			if (fd < 0) {
				printf ("\nFailed enable PWM_2\n");
				return -1;
			}
			write(fd, "1", 2);

			close(fd);
			return 0;
			break;
			}
		default:{
			printf("wrong PWM channel input\n");
			return -1;
		}
		}
}

/* PWM disable */
int pwm_disable(unsigned int pwm)
{
	int fd;

		switch(pwm){
		case 1:{
			    fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm0/enable", O_WRONLY);
				if (fd < 0) {
					printf ("\nFailed disable PWM_1\n");
					return -1;
				}
				write(fd, "0", 2);

				close(fd);
				return 0;
				break;
			}
			case 2:{
				fd = open(SYSFS_PWM_DIR "/pwmchip0/pwm1/enable", O_WRONLY);
				if (fd < 0) {
					printf ("\nFailed disable PWM_2\n");
					return -1;
				}
				write(fd, "0", 2);

				close(fd);
				return 0;
				break;
				}
			default:{
				printf("wrong PWM channel input\n");
				return -1;
			}
			}
}

/*
 * 在目录: /sys/class/pwm/pwmchip0
 * 创建控制风扇的设备节点
 */
int fan_dev_create(void)
{
	int ret = 0;
	
	ret = pwm_export(1);
	if(ret != 0) return ret;
	usleep(100);
	ret = pwm_export(2);
	if(ret != 0) return ret;
	usleep(100);

	ret = pwm_config(1, 1000000, 500000);
	if(ret != 0) return ret;
	usleep(100);
	ret = pwm_config(2, 1000000, 500000);
	if(ret != 0) return ret;

	return 0;
}


