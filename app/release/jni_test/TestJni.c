/*************************************************************************
    > File Name: Testjni.c
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
#include "./updateinfo.h"
#include "./scan_exuart.h"

#define KEYPAD_DEV "/dev/i2c-0"
#define UART_DEV "/dev/ttymxc1"
#define EX_CARD "/dev/other_gpio0"
#define UART_GPIO "/dev/uart_gpio0"
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

	//i2c_smbus_read_byte_data(oled_fd, 0x1); //清屏 
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
 * @jint ex_ch: 设备所在的扩展通道; 0：内部串口; 1~4: 为外部扩展口
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeAllChannelStatus
  (JNIEnv *env, jobject obj, jstring jstr, jint ex_ch)
{
	int len;
	const jbyte *s_chan;
	//const jbyte *s_time;
	char *s_time;
	char channel[1] = {0};
	char channel_val[16] = {0};

	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};

	FILE *cir_filep;
    dictionary * ini ;
    char       * ini_name = "/root/pavt/ini/pavt.ini";

	s_chan = (*env)->GetStringUTFChars(env, jstr, NULL);
	if(s_chan == NULL){
		return -CH_SET_ERR;
	}
	printf("%s, %d\n", s_chan, ex_ch);

	ini = iniparser_load(ini_name);
	iniparser_dump(ini, stdout);
	s_time = iniparser_getstring(ini, "fan:rel_time", NULL);
	printf("%s\n", s_time);
	
	sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_DATA_%s_%s>", ex_ch, s_chan, s_time);
	printf("w_buf: %s\n", w_buf);

	if(ex_ch == 0){
		ret = rs232_write(w_buf);
		if(ret < 0)
			return -CH_SET_ERR;
	}else{
		ret = rs485_write((1 << (ex_ch - 1)), w_buf);
		if(ret < 0)
			return -CH_SET_ERR;
	}

	return OK;

CH_SET_ERROR:
	printf("changeChannelStatus error!\n");
	return -CH_SET_ERR;

}

/*
 * 修改单个通道状态
 * Class:     TestJni
 * Method:    changeChannelStatus
 * Signature: (III)Z
 * @jint num: 通道号
 * @jint statu: 状态值
 * @jint ex_ch: 设备所在的扩展通道; 0：内部串口; 1~4: 为外部扩展口
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeChannelStatus
  (JNIEnv *env, jobject obj, jint num, jint statu, jint ex_ch)
{
	char channel[1] = {0};
	int i, ret;

	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char s_num[2] = {0};

	num += 1;
	if(num < 10){
		sprintf(s_num, "0%d", num);		
		printf("%s\n", s_num);
	}else{
		sprintf(s_num, "%d", num);		
	}
	sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_ONE_%s%d>", ex_ch, s_num, statu);
	printf("w_buf: %s\n", w_buf);

	if(ex_ch == 0){
		ret = rs232_write(w_buf);
		if(ret < 0)
			return -CH_SET_ERR;
	}else{
		ret = rs485_write((1 << ex_ch), w_buf);
		if(ret < 0)
			return -CH_SET_ERR;
	}

	return OK;

CH_SET_ERROR:
	printf("changeChannelStatus error!\n");
	return CH_SET_ERR;
}

/* 修改所有空开 
 * Class:     TestJni
 * Method:    changeAllAirStatus
 * Signature: (Ljava/lang/String;I)Z
 * @jint addr: 空开的从机地址
 * @jstring jstr: 空开的状态
 * @jint ex_ch:	空开所在的扩展通道
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeAllAirStatus
  (JNIEnv *env, jobject obj, jint addr, jstring jstr, jint ex_ch) 
{
	const jbyte *s_chan;
	char s_addr[2] = {0};

	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	
	s_chan = (*env)->GetStringUTFChars(env, jstr, NULL);
	if(s_chan == NULL){
		return -CH_SET_ERR;
	}
	printf("%s, %d\n", s_chan, ex_ch);
	
	if(addr < 10)
		sprintf(s_addr, "0%d", addr);
	else
		sprintf(s_addr, "%d", addr);


	sprintf(w_buf, "<AT_ALL_ADDR_%s_%s>", s_addr, s_chan);
	printf("w_buf: %s\n", w_buf);

	//ret = rs485_write((1 << (ex_ch - 1)), w_buf);
	ret = rs485_write(2, w_buf);

	return OK;
}

/* 修改单个空开 
 * Class:     TestJni
 * Method:    changeAirStatus
 * Signature: (III)Z
 * @jint addr: 空开的从机地址
 * @jint num: 需要控制的空开序号
 * @jint statu: 空开的状态
 * @jint ex_ch:	空开所在的扩展通道
 */
JNIEXPORT jboolean JNICALL Java_TestJni_changeAirStatus
  (JNIEnv *env, jobject obj, jint addr, jint num, jint statu, jint ex_ch)
{
	const jbyte *s_chan;
	char s_addr[2] = {0};

	int i, ret;
	char w_buf[128] = {0};

	if(addr < 10)
		sprintf(s_addr, "0%d", addr);
	else
		sprintf(s_addr, "%d", addr);

	if(num < 10)
		sprintf(w_buf, "<AT_ONE_ADDR_%s_CH0%d/%d>", s_addr, num, statu);
	else
		sprintf(w_buf, "<AT_ONE_ADDR_%s_CH%d/%d>", s_addr, num, statu);
	printf("w_buf: %s\n", w_buf);

	//ret = rs485_write((1 << (ex_ch - 1)), w_buf);
	ret = rs485_write(2, w_buf);

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
}


/*
 * 获取GPS信息:
 * Class:     TestJni
 * Method:    getGPSStatusNative
 * Signature: ()Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jstring JNICALL Java_TestJni_getGPSStatusNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};
	char temp[128] = {0};
	char temp_buf[128] = {0};
	char channel[1] = {0};

	char data[20][20] = {0} ;
	char *p = NULL;
	char *p_temp = NULL;
	jstring jstr;
	char val[32] = "lux error";

	jstr = (*env)->NewStringUTF(env, val);

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%x_LUX_GPS_DATA>", ex_ch);	//发送查询光照GPS指令
	rs485_write((1 << (ex_ch -1)), w_buf);
	printf("write data: %s\n", w_buf);
	ret = rs485_read((1 << (ex_ch - 1)), r_buf, 2);
	if(ret < 0){
		printf("data error or time out!\n");
		goto GPS_ERROR;
	}
	printf("recv data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		printf("data error!\n");
		goto GPS_ERROR;
	}

	p = strchr(r_buf, '$');
	strcpy(temp, p);
	printf("data: %s\n", temp);

	//if(strncmp(temp, "$GNGGA", 6)){
	if(strncmp(temp, "$GNRMC", 6)){
		printf("gps data error!\n");
		goto GPS_ERROR;
	}

	/* 获取gps信息
	 * data[1]: UTC时间，格式:hhmmss.sss
	 * data[2]: 定位有效标识: V:无效; A:有效
	 * data[3]: 纬度
	 * data[5]: 经度
	 * data[9]: UTC日期，格式:ddmmyy
	 * 其他值可忽略
	 */
	i = 1;
	while(1)
	{
		//printf("**************\n");
		p = strchr(temp, ',');
		if(p == NULL) break;
		sprintf(temp_buf, "%s", p+1);
		sprintf(temp, "%s", p+1);
		//printf("temp_buf: %s\n", temp_buf);
		p_temp = temp_buf;

		p = strchr(temp_buf, ',');
		if(p == NULL) break;
		*p = '\0';
		strcpy(data[i], temp_buf);
		printf("data[%d]: %s\n", i, data[i]);
		//printf("temp_buf: %s\n", p_temp);

		i++;
	}

	sprintf(val, "%s-%s", data[9], data[1]);
	printf("time: %s\n", val);

	jstr = (*env)->NewStringUTF(env, val);
	return jstr;

GPS_ERROR:
	jstr = (*env)->NewStringUTF(env, val);
	return jstr;
}

/*
 * 设置温湿度控制器温度
 * Class:     TestJni
 * Method:    setTmp
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_TestJni_setTmp
  (JNIEnv *env, jobject obj, jint tmp)
{
	return 0;	
}


/*
 * 重新扫描扩展口接入设备
 * Class:     TestJni
 * Method:    ScanExuartNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_TestJni_ScanExuartNative
  (JNIEnv *env, jclass obj)
{
	scan_exuart();
}

/*
 * 更新外设数据
 * Class:     TestJni
 * Method:    UpdateData
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_TestJni_UpdateData
  (JNIEnv *env, jclass obj)
{
	update_data();
}


/*
 * 更新柜门开关状态
 * Class:     TestJni
 * Method:    UpdataDoor
 * Signature: (I)Z
 */
JNIEXPORT jint JNICALL Java_TestJni_UpdataDoor
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_DOOR>", ex_ch);	//发送查询回路驱动信息
	rs485_write((1 << (ex_ch - 1)), w_buf);
	printf("get_switch_data: ex%d send data: %s\n", ex_ch, w_buf);
	ret = rs485_read((1 << (ex_ch - 1)), r_buf, 2);
	if(ret < 0) 
		return -1;

	ret = strncmp(r_buf, "<DOOR_alarm>", strlen("<DOOR_alarm>"));
	if(ret == 0)  
		return 1;
	ret = strncmp(r_buf, "<DOOR_close>", strlen("<DOOR_close>"));
	if(ret == 0)
		return 0;

	return 0;
}

/*
 * 获取4g信号强度
 * Class:     TestJni
 * Method:    get4gsignal
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_TestJni_get4gsignal
  (JNIEnv *env, jclass obj)
{
	int i;
	int signal = 0;
	int sig_fd, ret;
	int baud = 115200;
	int databits = 8;
	int stopbits = 1;
	char parity = 'N';
	char w_buf[20] = {0};
	char r_buf[100] = {0};
	char *p = NULL;

	sig_fd = open("/dev/ttyUSB2", O_RDWR);
	if(sig_fd < 0){
		printf("open ttyUSB2 error!\n");
		return -1;
	}
	ret = set_port(sig_fd, baud, databits, parity, stopbits);
	if(ret < 0){
		printf("set port failed\n");
		close(sig_fd);
		return -1;
	}

	strcpy(w_buf, "AT+CSQ\r");
	printf("check 4g signal: %s\n", w_buf);
	write(sig_fd, w_buf, strlen(w_buf));
	for(i = 0; i < 10; i++)
	{
		read(sig_fd, r_buf, sizeof(r_buf));
		printf("r_buf: %s\n", r_buf);

		p = strstr(r_buf, "+CSQ:");
		if(p != NULL) {
			p = strrchr(r_buf, ':');
			sprintf(r_buf, "%s", p + 2);
			//printf("read_buf:%s\n", r_buf);

			p = strchr(r_buf, ',');
			*p = '\0';
			//printf("read_buf:%s\n", r_buf);
			signal = atoi(r_buf);
			printf("4g signal: %d\n", signal);
			return signal;
		}
	}

	return -1;	
}

/*
 * 获取4G基站时间 
 * Class:     TestJni
 * Method:    Get4gTime
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_TestJni_Get4gTime
  (JNIEnv *env, jclass obj)
{
	int i;
	int signal = 0;
	int sig_fd, ret;
	int baud = 115200;
	int databits = 8;
	int stopbits = 1;
	char parity = 'N';
	char w_buf[20] = {0};
	char r_buf[100] = {0};
	char *p = NULL;
	jstring jstr;

	jstr = (*env)->NewStringUTF(env, val);

	sig_fd = open("/dev/ttyUSB2", O_RDWR);
	if(sig_fd < 0){
		printf("open ttyUSB2 error!\n");
		return -1;
	}
	ret = set_port(sig_fd, baud, databits, parity, stopbits);
	if(ret < 0){
		printf("set port failed\n");
		close(sig_fd);
		return -1;
	}

	strcpy(w_buf, "AT+CCLK?\r");
	printf("get 4g time: %s\n", w_buf);
	write(sig_fd, w_buf, strlen(w_buf));

	read(sig_fd, r_buf, sizeof(r_buf));
	printf("r_buf: %s\n", r_buf);
}

/*
 * 重启4g模块
 * Class:     TestJni
 * Method:    Restart4gModel
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_TestJni_Restart4gModel
  (JNIEnv *env, jclass obj)
{
	int fd;

	system("/usr/bin/killall pppd");

	fd = open("/dev/other_gpio0", O_RDWR);
	if(fd < 0){
		printf("open dev error!\n");
		return -1;
	}

	ioctl(fd,  IOREQ_CARDA_1_OFF);
	sleep(10);
	ioctl(fd,  IOREQ_CARDA_1_EN);
	sleep(10);

	system("/usr/sbin/pppd call gprs&");
	sleep(10);
	return 0;
}


#if 0 
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
	int uart_fd = 0, gpio_fd = 0;
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

	gpio_fd = open(UART_GPIO, O_RDWR, 0);
	if(gpio_fd < 0){
		printf(" Open gpio_fd dev errir!\n");
		goto LIG_GET_ERROR;
	}
	
	//init uart (ttyO3)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto LIG_GET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto LIG_GET_ERROR;
	}

	sprintf(channel, "%d", ex_ch);
	write(gpio_fd, channel, sizeof(ex_ch));		//set uart addr(0~12)

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

	close(gpio_fd);
	close(uart_fd);
	return jstr;

LIG_GET_ERROR:
	close(gpio_fd);
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
	int uart_fd = 0, gpio_fd = 0;
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
	
	gpio_fd = open(UART_GPIO, O_RDWR, 0);
	if(gpio_fd < 0){
		printf(" Open gpio_fd dev errir!\n");
		goto POW_GET_ERROR;
	}
	
	//init uart (ttyO3)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto POW_GET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto POW_GET_ERROR;
	}

	sprintf(channel, "%d", ex_ch);
	write(gpio_fd, channel, sizeof(ex_ch));		//set uart addr(0~12)

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

	close(gpio_fd);
	close(uart_fd);
	return texts;

POW_GET_ERROR:
	close(gpio_fd);
	close(uart_fd);
	return texts;
}

/*
 * 获取回路驱动信息
 * Class:     TestJni
 * Method:    getCircuitNative
 * Signature: (I)[Ljava/lang/String;
 @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jobjectArray JNICALL Java_TestJni_getCircuitNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	int uart_fd = 0, gpio_fd = 0;
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


	gpio_fd = open(UART_GPIO, O_RDWR, 0);
	if(gpio_fd < 0){
		printf(" Open gpio_fd dev errir!\n");
		goto CIR_GET_ERROR;
	}
	
	//init uart (ttyO3)
	uart_fd = open(UART_DEV, O_RDWR, 0);
	if(uart_fd < 0){
		printf(" Open uart_fd dev errir!\n");
		goto CIR_GET_ERROR;
	}
	if(setup_port(uart_fd, baud, databits, parity, stopbits)){
		printf("setup port error!\n");
		goto CIR_GET_ERROR;
	}

	sprintf(channel, "%d", ex_ch);
	write(gpio_fd, channel, sizeof(ex_ch));		//set uart addr(0~12)

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

	close(gpio_fd);
	close(uart_fd);
	return texts;

CIR_GET_ERROR:
	close(gpio_fd);
	close(uart_fd);
	return texts;
}
#endif 
