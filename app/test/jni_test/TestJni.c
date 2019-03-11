/*************************************************************************
    > File Name: pavt_jni.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年12月13日 星期三 15时32分04秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <termio.h>
#include <time.h>
#include <jni.h>

#include "serial.h"
#include "inifile.h"
#include "TestJni.h"
#include "other_gpio.h"
#include "i2c-dev.h"
#include "pavt_error.h"
#include "dictionary.h"
#include "iniparser.h"


#define KEYPAD_DEV "/dev/i2c-1"
#define UART_DEV "/dev/ttymxc2"
#define EX_CARD "/dev/other_gpio0"
#define I2C_ADDR 0x50

/* for test */
JNIEXPORT void JNICALL Java_TestJni_print
  (JNIEnv *env, jobject obj, jstring text)
{
	const jbyte *str=(const jbyte *)(*env)->GetStringUTFChars(env, text, JNI_FALSE);
	printf("Hello------>%s\n", str);

	(*env)->ReleaseStringUTFChars(env, text, (const char *)str);

	return;
}


/*
 * 更新整屏数据
 * Class:	  TestJni
 * Method:    printTextNative
 * Signature: (Ljava/lang/String;)Z
 * @jstring str：数据内容
 */
JNIEXPORT jboolean JNICALL Java_TestJni_printTextNative
  (JNIEnv *env, jclass obj, jstring prompt)
{
	int i, j;
	int oled_fd = 0;
	int len, ret, flag;
	const jbyte *str;
	char w_buf[4][16] = {{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
				          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
					      },
						 {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
				          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
					      },
						 {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
				          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
					      },
						 {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
				          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
					      },
						 };

	str = (*env)->GetStringUTFChars(env, prompt, NULL);
	if (str == NULL) {
		return -OLED_ERR; /* OutOfMemoryError already thrown */
	}
	printf("%s\n", str);

	oled_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(oled_fd < 0){
		printf("open dev error!\n");
		return -OLED_ERR;
	}
	if(ioctl(oled_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		printf(" set slave addr error!\n");
		goto OLED_ERROR;
	}

	ret = strlen(str);
	if(ret > 64){
		printf("date error[lenth: %d]!\n", ret);
		goto OLED_ERROR;
	}
	printf("data lenth: %d\n", ret);

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 16; j++)
		{
			strncpy(&w_buf[i][j], str, sizeof(char));
			*str ++;
			if(*str == '$'){
				*str ++;
				break;
			}
			if(*str == '\0') break;
		}
		printf("lenth: %ld, val: %s\n", sizeof(w_buf[i]), w_buf[i]);
	}

	i2c_smbus_read_byte_data(oled_fd, 0x1); //清屏 
	usleep(15*1000);
	while(1)
	{
		printf("************* 1 ******************\n");
		i2c_smbus_write_i2c_block_data(oled_fd, 0x6, sizeof(w_buf[0]), w_buf[0]); //第一行数据地址：0x06
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		printf("************* 2 ******************\n");
		i2c_smbus_write_i2c_block_data(oled_fd, 0x7, sizeof(w_buf[1]), w_buf[1]); //第二行数据地址：0x07
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		printf("************* 3 ******************\n");
		i2c_smbus_write_i2c_block_data(oled_fd, 0x8, sizeof(w_buf[2]), w_buf[2]); //第三行数据地址：0x08
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		printf("************* 4 ******************\n");
		i2c_smbus_write_i2c_block_data(oled_fd, 0x9, sizeof(w_buf[3]), w_buf[3]); //第四行数据地址：0x09
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}

	//(*env)->ReleaseStringUTFChars(env, prompt, str);

	i2c_smbus_read_byte_data(oled_fd, 0x2); //心跳信号
	close(oled_fd);
	return OK;

OLED_ERROR:
	close(oled_fd);
	return -OLED_ERR;
}


/*
 * 更新指定的单个字符
 * Class:     TestJni
 * Method:    printTextWithPosNative
 * Signature: (IIC)Z
 * @jint x: 屏幕x坐标(0~15)
 * @jint y: 屏幕y坐标(0~3)
 * @jchar c: 更新的值
 */
JNIEXPORT jboolean JNICALL Java_TestJni_printTextWithPosNative
  (JNIEnv *env, jclass obj, jint x, jint y, jchar c)
{
	int oled_fd, ret;
	char w_buf[3] = {0};
	int flag;
		
	if(x < 0 | x > 15){
		printf("x rang: 0~15");
		return -OLED_ERR;
	}
	if( y < 0 | y > 3){
		printf("y rang: 0~3");
		return -OLED_ERR;
	}

	w_buf[0] = x;
	w_buf[1] = y;
	w_buf[2] = c;
	printf("x: %d, y: %d, c: %c\n", x, y, c);

	oled_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(oled_fd < 0){
		printf("open dev error!\n");
		goto OLED_ERROR;
	}
	if(ioctl(oled_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		printf(" set slave addr error!\n");
		goto OLED_ERROR;
	}

	while(1)
	{
		i2c_smbus_write_i2c_block_data(oled_fd, 0x3, sizeof(w_buf), w_buf);  //更新单个字符地址：0x03
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}

	i2c_smbus_read_byte_data(oled_fd, 0x2); //心跳信号
	close(oled_fd);
	return OK;

OLED_ERROR:
	close(oled_fd);
	return -OLED_ERR;
}

/*
 * 修改所有led颜色 
 * Class:     TestJni
 * Method:    changeAllLedStatus
 * Signature: ([I)Z
 * @arry: 16位对应通道颜色值
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeAllLedStatus
  (JNIEnv *env, jobject obj, jintArray arry)
{
	int led_fd, ret;
	int i;
	int len;
	int *add_arry;
	char led_status[16] = {0};

	led_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(led_fd < 0){
		printf("open dev error!\n");
		goto LED_ERROR; 
	}
	if(ioctl(led_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		printf(" set slave addr error!\n");
		goto LED_ERROR; 
	}

	//获取数组长度
	len = (*env)->GetArrayLength(env, arry);
	if(len > 16){
		printf("led data error!\n");
		goto LED_ERROR; 
	}

	//获取数组首地址
	add_arry = (*env)->GetIntArrayElements(env, arry,0);
		
	for(i = 0; i < len; i++)
	{
		led_status[i] = add_arry[i];
		//printf("led_status[%d]: %d\n", i, led_status[i]);
	}
	//printf("lenth: %d\n", sizeof(led_status));
	
	while(1)
	{
		i2c_smbus_write_i2c_block_data(led_fd, 0x5, sizeof(led_status), led_status); //更新所有led地址：0x5
		ret = i2c_smbus_read_byte_data(led_fd, 0x1f); // 
		if(ret == 1)
			break;
	}

	close(led_fd);
	return OK;

LED_ERROR:
	close(led_fd);
	return  -LED_ERR;
}


/*
 * 修改指定led颜色 
 * Class:     TestJni
 * Method:    changeLedStatus
 * Signature: (III)Z
 * @jint x: led x坐标(0~8)
 * @jint x: led y坐标(0~1)
 * @jint x: led 颜色值: 0x0 关闭; 0x1 红色; 0x2 绿色; 0x3 蓝色;
 *						0x4 黄色; 0x5 粉色; 0x6 青色; 0x7 白色;
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeLedStatus
  (JNIEnv *env, jobject obj, jint x, jint y, jint c)
{
	int led_fd, ret;
	char w_buf[3] = {0};
		
	if(x < 0 | x > 8){
		printf("x rang: 0~8");
		return -LED_ERR;
	}
	if( y < 0 | y > 1){
		printf("y rang: 0~1");
		return -LED_ERR;
	}

	w_buf[0] = x;
	w_buf[1] = y;
	w_buf[2] = c;
	printf("x: %d, y: %d, c: %d\n", x, y, c);

	led_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(led_fd < 0){
		printf("open dev error!\n");
		goto LED_ERROR;
	}
	if(ioctl(led_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		printf(" set slave addr error!\n");
		goto LED_ERROR;
	}

	while(1)
	{
		i2c_smbus_write_i2c_block_data(led_fd, 0x4, sizeof(w_buf), w_buf); //更新单个led地址：0x4
		ret = i2c_smbus_read_byte_data(led_fd, 0x1f); // 
		if(ret == 1)
			break;
	}

	close(led_fd);
	return OK;
	
LED_ERROR:
	close(led_fd);
	return  -LED_ERR;
}


/*
 * 修改全部通道状态
 * Class:     TestJni
 * Method:    changeAllChannelStatus
 * Signature: ([I)Z
 * @jstring jstr: 全部通道状态值
 * @jstring time: 打开通道间隔时间
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeAllChannelStatus
  (JNIEnv *env, jobject obj, jstring jstr, jstring time, jint ex_ch)
{
	int len;
	const jbyte *str;
	const jbyte *s_time;
	char channel[1] = {0};
	char channel_val[16] = {0};

	int i, ret;
	int uart_fd = 0;
	int baud = 9600;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;
	char w_buf[128] = {0};
	char r_buf[128] = {0};

	FILE *cir_filep;
    dictionary * ini ;
    char       * ini_name = "./cir_data.info";

	str = (*env)->GetStringUTFChars(env, jstr, NULL);
	if(str == NULL){
		return -CH_SET_ERR;
	}
	printf("%s\n", str);

	s_time = (*env)->GetStringUTFChars(env, time, NULL);
	if(str == NULL){
		return -CH_SET_ERR;
	}
	printf("%s\n", s_time);
	
	
	//init uart (ttymxc2)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto CH_SET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto CH_SET_ERROR;
	}

	strncpy(w_buf, str, strlen(str));
	write_data(uart_fd, w_buf, strlen(w_buf));

	close(uart_fd);
	return OK;

CH_SET_ERROR:
	close(uart_fd);
	return CH_SET_ERR;

}

/*
 * 修改单个通道状态
 * Class:     TestJni
 * Method:    changeChannelStatus
 * Signature: (III)Z
 * jint num: 通道号
 * jint statu: 状态值
 * jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeChannelStatus
  (JNIEnv *env, jobject obj, jint num, jint statu, jint ex_ch)
{
	return OK;
}

/*
 * 获取当前所有通道状态
 * Class:     TestJni
 * Method:    getChannelStatusNative
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_TestJni_getChannelStatusNative
  (JNIEnv *env, jclass obj, jint lenth)
{
	int i;
	jintArray  arry;
	int vals[16] = {0};

	for(i = 0; i < 8; i++)
		vals[i] = i;

	arry = (*env)->NewIntArray(env, lenth);
	(*env)->SetIntArrayRegion(env, arry, 0, lenth, vals);

	return arry;

}

/*
 * 获取指定通道状态
 * Class:     TestJni
 * Method:    getChannelNumNative
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_TestJni_getChannelNumNative
  (JNIEnv *env, jclass obj)
{
	int channel_statu;
	channel_statu = 8;

	return channel_statu;
}


/*
 * 蜂鸣器：
 * Class:     TestJni
 * Method:    BuzzerNative
 * Signature: (I)Z
 * jint val: 0 关闭; 1 打开
 */
JNIEXPORT jboolean JNICALL Java_TestJni_BuzzerNative
  (JNIEnv * env, jclass obj, jint val)
{
	int ret; 
	int fd;

	fd = open(EX_CARD, O_RDWR, 777);
	if(fd < 0){
		printf("open excard error!\n");
		goto BUZ_ERROR;
	}

	//printf("val: %d\n", val);
	switch(val)
	{
		case 0:
			ioctl(fd, IOREQ_BUZZER_OFF);
		break;
		case 1:
			ioctl(fd, IOREQ_BUZZER_ON);
		break;
	}

	close(fd);
	return OK;

BUZ_ERROR:	
	close(fd);
	return BUZ_ERR;
#if 0
	const jbyte *str;
	int fd_export, fd_direction, fd_value;

	(*env)->ReleaseStringUTFChars(env, val, str);

	if(access("/sys/class/gpio/gpio86", F_OK)){
		fd_export = open("/sys/class/gpio/export", O_RDWR, 0);	
		if(fd_export < 0){
			printf("open export error!\n");
			return -1;
		}
		write(fd_export, "86", strlen("86"));
		close(fd_export);

		fd_direction = open("/sys/class/gpio/gpio86/direction", O_RDWR, 0);
		if(fd_direction < 0){
			printf("open direction error!\n");
			return -1;
		}
		write(fd_direction, "out", strlen("out"));
		close(fd_direction);
	}else{
		fd_value = open("/sys/class/gpio/gpio86/value", O_RDWR, 0);
		if(fd_value < 0){
			printf("open value error!\n");
			return -1;
		}
		write(fd_value, str, sizeof(val));
		close(fd_value);
	}

	(*env)->ReleaseStringUTFChars(env, val, str);
#endif
}


/*
 * 获取GPS信息:
 * Class:     TestJni
 * Method:    getGPSStatusNative
 * Signature: ()Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jfloatArray JNICALL Java_TestJni_getGPSStatusNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int lenth = 2;
	jstring jstr;
	char val1[32] = "gps error";
	char val2[32] = "gps error";
	char *vals[2] = {val1, val2} ;
	char *p = NULL;

	int i, ret;
	int uart_fd = 0;
	int baud = 9600;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char channel[1] = {0};


	jclass objClass = (*env)->FindClass(env, "java/lang/String");
	jobjectArray texts = (*env)->NewObjectArray(env, (jsize)2, objClass, 0);
	for(i = 0; i < 2; i++)
	{
		jstr = (*env)->NewStringUTF(env, vals[i]);
		(*env)->SetObjectArrayElement(env, texts, i, jstr);
	}

	//init uart (ttymxc2)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto GPS_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto GPS_ERROR;
	}

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT+CHECK%d_LUX_GPS_DATA>", ex_ch);	//发送查询光照GPS指令
	write_data(uart_fd, w_buf, strlen(w_buf));
	//printf("write data: %s\n", w_buf);
	ret = read_data(uart_fd, r_buf, sizeof(r_buf), 5*1000);
	if(ret < 0){
		printf("data error or time out!\n");
		goto GPS_ERROR;
	}
	//printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		goto GPS_ERROR;
	}

	p = strrchr(r_buf, '>');
	*p = '\0';							//除去最后的'>'

	p = strrchr(r_buf, '_');
	//sprintf(val1, "%s", p+1);
	strncpy(val1, p+1, strlen(p+1));
	*p = '\0';
	//printf("val1:%s\n", val1);			//获取第一个数据

	p = strrchr(r_buf, '_');
	//sprintf(val2, "%s", p+1);
	strncpy(val2, p+1, strlen(p+1));
	*p = '\0';
	//printf("val2:%s\n", val2);			//获取第二个数据


	for(i = 0; i < 2; i++)
	{
		jstr = (*env)->NewStringUTF(env, vals[i]);
		(*env)->SetObjectArrayElement(env, texts, i, jstr);
	}

	close(uart_fd);
	return texts;

GPS_ERROR:
	close(uart_fd);
	return texts;
}

/*
 * 获取光照强度:
 * Class:     TestJni
 * Method:    getLightIntensityNative
 * Signature: (I)F
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jstring JNICALL Java_TestJni_getLightIntensityNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	int uart_fd = 0;
	int baud = 9600;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char channel[1] = {0};

	jstring jstr;
	char val[32] = "lux error";
	char *p = NULL;

	jstr = (*env)->NewStringUTF(env, val);

	//init uart (ttymxc2)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto LIG_GET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto LIG_GET_ERROR;
	}

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT+CHECK%d_LUX_GPS_DATA>", ex_ch);	//发送查询光照GPS指令
	write_data(uart_fd, w_buf, strlen(w_buf));
	printf("write data: %s\n", w_buf);
	ret = read_data(uart_fd, r_buf, sizeof(r_buf), 5*1000);
	if(ret < 0){
		printf("data error or time out!\n");
		goto LIG_GET_ERROR;
	}
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		goto LIG_GET_ERROR;
	}

	p = strrchr(r_buf, '>');
	*p = '\0';							//除去最后的'>'


	p = strrchr(r_buf, '_');
	*p = '\0';
	p = strrchr(r_buf, '_');
	*p = '\0';							//除去gps数据

	p = strrchr(r_buf, '_');
	strncpy(val, p+1, strlen(p+1));
	printf("val:%s\n", val);			//获取光照数据
	
	jstr = (*env)->NewStringUTF(env, val);

	close(uart_fd);
	return jstr;

LIG_GET_ERROR:
	close(uart_fd);
	return jstr;
}


/*
 * 获取电能表数据
 * Class:     TestJni
 * Method:    getPowerNative
 * Signature: (I)[Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jobjectArray JNICALL Java_TestJni_getPowerNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	int uart_fd = 0;
	int baud = 9600;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	//char r_buf[128] = "<POWER_123456.0_[A/220/000.0][B/220/000.0][C/220/000.0]>";
	char channel[1] = {0};

	int lenth = 3;
	jstring jstr;
	char vals[4][32] = {0} ;
	char *p = NULL;
	
	jclass objClass = (*env)->FindClass(env, "java/lang/String");
	jobjectArray texts = (*env)->NewObjectArray(env, (jsize)4, objClass, 0);
	
	//init uart (ttymxc2)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto POW_GET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto POW_GET_ERROR;
	}

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT+CHECK%d_POWER>", ex_ch);	//发送查询电能表指令
	write_data(uart_fd, w_buf, strlen(w_buf));
	printf("write data: %s\n", w_buf);
	ret = read_data(uart_fd, r_buf, sizeof(r_buf), 5*1000);
	if(ret < 0){
		printf("data error or time out!\n");
		goto POW_GET_ERROR;
	}
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		goto POW_GET_ERROR;
	}

	//获取三相电电流电压:val[0]~val[2]
	for(i = 0; i < 3; i++)
	{
		p = strrchr(r_buf, ']');
		*p = '\0';
		p = strrchr(r_buf, '[');
		strncpy(vals[i], p+1, strlen(p+1));
		*p = '\0';
	}

	//获取功率:val[3]
	p = strrchr(r_buf, '_');
	*p = '\0';
	p = strrchr(r_buf, '_');
	strncpy(vals[3], p+1, strlen(p+1));

	for(i = 0; i < 4; i++)
	{
		jstr = (*env)->NewStringUTF(env, vals[i]);
		(*env)->SetObjectArrayElement(env, texts, i, jstr);
	}

	close(uart_fd);
	return texts;

POW_GET_ERROR:
	close(uart_fd);
	return texts;
}

/*
 * 获取回路驱动信息
 * Class:     TestJni
 * Method:    getCircuitNative
 * Signature: (I)[Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jobjectArray JNICALL Java_TestJni_getCircuitNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	int uart_fd = 0;
	int baud = 9600;
	int databits = 8;
	int stopbits = 1;
	int parity = 0;
	char w_buf[128] = {0};
	char r_buf[512] = {0};
	//char r_buf[512] = "<CIRCUIT_ON/OFF_DATA_[001/230/10.10/1][002/230/10.10/1][003/230/10.10/1][004/230/10.10/1][005/230/10.10/1][006/230/10.10/1][007/230/10.10/1][008/230/10.10/1][009/230/10.10/1][010/230/10.10/1][011/230/10.10/1][012/230/10.10/1][013/230/10.10/1][014/230/10.10/1][015/230/10.10/1][016/230/10.10/1]> ";
	char channel[1] = {0};

	int lenth = 16;
	jstring jstr;
	char vals[16][32] = {0} ;
	char *p = NULL;

	jclass objClass = (*env)->FindClass(env, "java/lang/String");
	jobjectArray texts = (*env)->NewObjectArray(env, (jsize)16, objClass, 0);


	//init uart (ttymxc2)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto CIR_GET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto CIR_GET_ERROR;
	}

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT+CHECK%d_CIRCUIT_ON/OFF_DATA>", ex_ch);	//发送查询回路驱动信息
	write_data(uart_fd, w_buf, strlen(w_buf));
	printf("write data: %s\n", w_buf);
	ret = read_data(uart_fd, r_buf, sizeof(r_buf), 5*1000);
	if(ret < 0){
		printf("data error or time out!\n");
		goto CIR_GET_ERROR;
	}
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		goto CIR_GET_ERROR;
	}

	//获取16路通道驱动信息
	for(i = 15; i >= 0; i--)
	{
		p = strrchr(r_buf, ']');
		*p = '\0';
		p = strrchr(r_buf, '[');
		strncpy(vals[i], p+1, strlen(p+1));
		*p = '\0';
	}
	//for(i = 0; i < 16; i++)
	//	printf("vals[%d]: %s\n", i, vals[i]);

	for(i = 0; i < 16; i++)
	{
		jstr = (*env)->NewStringUTF(env, vals[i]);
		(*env)->SetObjectArrayElement(env, texts, i, jstr);
	}

	close(uart_fd);
	return texts;

CIR_GET_ERROR:
	close(uart_fd);
	return texts;
}
