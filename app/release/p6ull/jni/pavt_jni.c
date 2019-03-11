/*************************************************************************
    > File Name: pavt_jni.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年12月13日 星期三 15时32分04秒
 ************************************************************************/
#include "pavt_jni.h"


/*
 * 更新整屏数据
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    printTextNative
 * Signature: (Ljava/lang/String;)Z
 * @jstring prompt：数据内容
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_printTextNative
  (JNIEnv *env, jclass obj, jstring prompt)
{
	int i, j;
	int oled_fd = 0;
	int len, ret, flag;
	const jbyte *str;
	char w_buf[4][16] = {0};

	str = (*env)->GetStringUTFChars(env, prompt, NULL);
	if (str == NULL) {
		return -OLED_ERR; /* OutOfMemoryError already thrown */
	}
	//jni_dbg("<jni>: %s\n", str);

	oled_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(oled_fd < 0){
		perror("<jni>: open dev error!\n");
		return -OLED_ERR;
	}
	if(ioctl(oled_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		perror("<jni>: set slave addr error!\n");
		goto OLED_ERROR;
	}

	ret = strlen(str);
	if(ret > 64){
		perror("<jni>: oled date error!\n");
		goto OLED_ERROR;
	}
	//jni_dbg("<jni>: data lenth: %d\n", ret);

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
		//jni_dbg("<jni>: lenth: %ld, val: %s\n", sizeof(w_buf[i]), w_buf[i]);
	}

	//i2c_smbus_read_byte_data(oled_fd, 0x1); //清屏 
	while(1)
	{
		i2c_smbus_write_i2c_block_data(oled_fd, 0x6, sizeof(w_buf[0]), w_buf[0]); //第一行数据地址：0x06
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		i2c_smbus_write_i2c_block_data(oled_fd, 0x7, sizeof(w_buf[1]), w_buf[1]); //第二行数据地址：0x07
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
		i2c_smbus_write_i2c_block_data(oled_fd, 0x8, sizeof(w_buf[2]), w_buf[2]); //第三行数据地址：0x08
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}
	usleep(15*1000);

	while(1)
	{
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
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    printTextWithPosNative
 * Signature: (IIC)Z
 * @jint x: 屏幕x坐标(0~15)
 * @jint y: 屏幕y坐标(0~3)
 * @jchar c: 更新的值
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_printTextWithPosNative
  (JNIEnv *env, jclass obj, jint x, jint y, jchar c)
{
	int oled_fd, ret;
	char w_buf[3] = {0};
	int flag;
		
	if(x < 0 | x > 15){
		perror("<jni>: x rang: 0~15");
		return -OLED_ERR;
	}
	if( y < 0 | y > 3){
		perror("<jni>: y rang: 0~3");
		return -OLED_ERR;
	}

	w_buf[0] = x;
	w_buf[1] = y;
	w_buf[2] = c;
	jni_dbg("<jni>: %s, x: %d, y: %d, c: %c\n", __func__, x, y, c);

	oled_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(oled_fd < 0){
		perror("<jni>: open dev error!\n");
		goto OLED_ERROR;
	}
	if(ioctl(oled_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		perror("<jni>:  set slave addr error!\n");
		goto OLED_ERROR;
	}

	while(1)
	{
		i2c_smbus_write_i2c_block_data(oled_fd, 0x3, sizeof(w_buf), w_buf);  //更新单个字符地址：0x03
		ret = i2c_smbus_read_byte_data(oled_fd, 0x1f); // 
		if(ret == 1)
			break;
	}

	//i2c_smbus_read_byte_data(oled_fd, 0x2); //心跳信号
	close(oled_fd);
	return OK;

OLED_ERROR:
	close(oled_fd);
	return -OLED_ERR;
}

/*
 * 修改所有led颜色 
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    changeAllLedStatus
 * Signature: ([I)Z
 * @arry: 16位对应通道颜色值
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_changeAllLedStatusNative
  (JNIEnv *env, jobject obj, jintArray arry)
{
	int led_fd, ret;
	int i;
	int len;
	int *add_arry;
	char led_status[16] = {0};

	led_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(led_fd < 0){
		perror("<jni>: open dev error!\n");
		goto LED_ERROR; 
	}
	if(ioctl(led_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		perror("<jni>:  set slave addr error!\n");
		goto LED_ERROR;
	}

	//获取数组长度
	len = (*env)->GetArrayLength(env, arry);
	if(len > 16){
		perror("<jni>: led data error!\n");
		goto LED_ERROR;
	}

	//获取数组首地址
	add_arry = (*env)->GetIntArrayElements(env, arry,0);
		
	for(i = 0; i < len; i++)
	{
		led_status[i] = add_arry[i];
	}
	//jni_dbg("lenth: %d\n", sizeof(led_status));
	
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
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    changeLedStatus
 * Signature: (III)Z
 * @jint x: led x坐标(0~8)
 * @jint x: led y坐标(0~1)
 * @jint x: led 颜色值: 0x0 关闭; 0x1 红色; 0x2 绿色; 0x3 蓝色;
 *						0x4 黄色; 0x5 粉色; 0x6 青色; 0x7 白色;
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_changeLedStatusNative
  (JNIEnv *env, jobject obj, jint x, jint y, jint c)
{
	int led_fd, ret;
	char w_buf[3] = {0};
		
	if(x < 0 | x > 8){
		perror("<jni>: x rang: 0~8");
		return -LED_ERR;
	}
	if( y < 0 | y > 1){
		perror("<jni>: y rang: 0~1");
		return -LED_ERR;
	}

	w_buf[0] = x;
	w_buf[1] = y;
	w_buf[2] = c;
	jni_dbg("<jni>: %s，x: %d, y: %d, c: %d\n", __func__, x, y, c);

	led_fd = open(KEYPAD_DEV, O_RDWR, 0);
	if(led_fd < 0){
		perror("<jni>: open dev error!\n");
		goto LED_ERROR;
	}
	if(ioctl(led_fd, I2C_SLAVE_FORCE, I2C_ADDR)){
		perror("<jni>:  set slave addr error!\n");
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
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    changeAllChannelStatus
 * Signature: ([I)Z
 * @jstring jstr: 全部通道状态值
 * @jint ex_ch: 设备所在的扩展通道; 0：内部串口; 1~4: 为外部扩展口
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_changeAllChannelStatusNative
  (JNIEnv *env, jobject obj, jstring jstr, jint ex_ch)
{
	int len;
	const jbyte *s_chan;
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
	//jni_dbg("<jni>: %s\n", s_chan);
	
	ini = iniparser_load(ini_name);
	s_time = iniparser_getstring(ini, "fan:rel_time", NULL);
	//jni_dbg("<jni>%s\n", s_time);

	sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_DATA_%s_%s>", ex_ch, s_chan, s_time);
	jni_dbg("<jni>: %s: %s\n", __func__, w_buf);

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
	perror("<jni> changeChannelStatus error!\n");
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
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_changeChannelStatusNative
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
	}else{
		sprintf(s_num, "%d", num);		
	}
	sprintf(w_buf, "<AT_CHECK%x_CIRCUIT_ON/OFF_ONE_%s%d>", ex_ch, s_num, statu);
	jni_dbg("<jni> %s: %s\n", __func__, w_buf);

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
	perror("<jni> changeChannelStatus error!\n");
	return CH_SET_ERR;
}

/*
 * 获取当前所有通道状态
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    getChannelStatusNative
 * Signature: ()[I
 */
//JNIEXPORT jintArray JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_getChannelStatusNative
//  (JNIEnv *env, jclass obj, jint lenth)
//{
//	int i;
//	jintArray  arry;
//	int vals[16] = {0};
//
//	for(i = 0; i < 8; i++)
//		vals[i] = i;
//
//	arry = (*env)->NewIntArray(env, lenth);
//	(*env)->SetIntArrayRegion(env, arry, 0, lenth, vals);
//
//	return arry;
//
//}

/*
 * 获取指定通道状态
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    getChannelNumNative
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_getChannelNumNative
  (JNIEnv *env, jclass obj)
{
	int channel_statu;
	channel_statu = 8;

	return channel_statu;
}


/*
 * 蜂鸣器：
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    BuzzerNative
 * Signature: (I)Z
 * jint val: 0 关闭; 1 打开
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_BuzzerNative
  (JNIEnv * env, jclass obj, jint val)
{
	int ret; 
	int fd;

	fd = open(EX_CARD, O_RDWR, 777);
	if(fd < 0){
		perror("<jni>: open excard error!\n");
		return -1;
	}

	//jni_dbg("<jni> %s: val: %d\n", __func__, val);
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
	return 0;
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
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    getGPSStatusNative
 * Signature: ()Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jobjectArray JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_getGPSStatusNative
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
	jni_dbg("<jni> CHECK GPS: %s\n", w_buf);
	ret = rs485_read((1 << (ex_ch - 1)), r_buf);
	if(ret < 0){
		perror("<jni> data error or time out!\n");
		goto GPS_ERROR;
	}
	jni_dbg("<jni> gps data: %s\n", r_buf);

	/* 解析数据 */
	if(strstr(r_buf, "error")){
		perror("<jni> gps data error!\n");
		goto GPS_ERROR;
	}

	p = strchr(r_buf, '$');
	strcpy(temp, p);
	//jni_dbg("data: %s\n", temp);

	//if(strncmp(temp, "$GNGGA", 6)){
	if(strncmp(temp, "$GNRMC", 6)){
		perror("<jni> gps data error!\n");
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
		p = strchr(temp, ',');
		if(p == NULL) break;
		sprintf(temp_buf, "%s", p+1);
		sprintf(temp, "%s", p+1);
		//jni_dbg("temp_buf: %s\n", temp_buf);
		p_temp = temp_buf;

		p = strchr(temp_buf, ',');
		if(p == NULL) break;
		*p = '\0';
		strcpy(data[i], temp_buf);
		//jni_dbg("data[%d]: %s\n", i, data[i]);
		//jni_dbg("temp_buf: %s\n", p_temp);

		i++;
	}

	sprintf(val, "%s-%s", data[9], data[1]);
	//jni_dbg("time: %s\n", val);

	jstr = (*env)->NewStringUTF(env, val);
	return jstr;

GPS_ERROR:
	jstr = (*env)->NewStringUTF(env, val);
	return jstr;
}

/*
 * 获取光照强度:
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    getLightIntensityNative
 * Signature: (I)F
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jstring JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_getLightIntensityNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	jstring jstr;
	char val[32] = "lux error";
	char *p = NULL;

	jstr = (*env)->NewStringUTF(env, val);

	return jstr;
}


/*
 * 获取电能表数据
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    getPowerNative
 * Signature: (I)[Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
//JNIEXPORT jstring JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_getPowerNative
//  (JNIEnv *env, jclass obj, jint ex_ch)
//{
//	jstring jstr;
//	char *p = NULL;
//	
//	jclass objClass = (*env)->FindClass(env, "java/lang/String");
//	jobjectArray texts = (*env)->NewObjectArray(env, (jsize)4, objClass, 0);
//	
//	return texts;
//}

/*
 * 获取回路驱动信息
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    getCircuitNative
 * Signature: (I)[Ljava/lang/String;
 * @jint ex_ch: 设备所在的扩展通道
 */
JNIEXPORT jobjectArray JNICALL Java_com_zjpavt_client_nativemethod_JniMethod_getCircuitNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	jstring jstr;
	char vals[16][32] = {0} ;
	char *p = NULL;

	jclass objClass = (*env)->FindClass(env, "java/lang/String");
	jobjectArray texts = (*env)->NewObjectArray(env, (jsize)16, objClass, 0);


	return texts;
}

/*
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    ScanExuartNative
 * Signature: ()Z
 * 重新扫描扩展口接入设备
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_ScanExuartNative
  (JNIEnv *env, jclass obj)
{
	int i = 0;

	for(i = 0; i < 3; i++)
	{
		scan_exuart();
	}
	return 0;
}

/*
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    updateExtendDeviceInfoNative
 * Signature: ()Z
 * 更新外设数据
 */
JNIEXPORT jboolean JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_updateExtendDeviceInfoNative
  (JNIEnv *env, jclass obj)
{
	update_data();
}

/*
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    updateDoorStatusNative
 * Signature: (I)Z
 * 更新柜门开关状态
 */
JNIEXPORT jint JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_updateDoorStatusNative
  (JNIEnv *env, jclass obj, jint ex_ch)
{
	int i, ret;
	char w_buf[128] = {0};
	char r_buf[128] = {0};

	bzero(w_buf, sizeof(w_buf));
	sprintf(w_buf, "<AT_CHECK%d_DOOR>", ex_ch);	//发送查询回路驱动信息
	rs485_write((1 << (ex_ch - 1)), w_buf);
	//jni_dbg("<jni> CHECK DOOR: ex%d send data: %s\n", ex_ch, w_buf);
	ret = rs485_read((1 << (ex_ch - 1)), r_buf);
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
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    get4GSignalNative
 * Signature: ()I
 * 获取4g信号强度
 */
JNIEXPORT jint JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_get4GSignalNative
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
		perror("open ttyUSB2 error!\n");
		return -1;
	}
	ret = set_port(sig_fd, baud, databits, parity, stopbits);
	if(ret < 0){
		perror("set port failed\n");
		close(sig_fd);
		return -1;
	}

	strcpy(w_buf, "AT+CSQ\r");
	write(sig_fd, w_buf, strlen(w_buf));
	for(i = 0; i < 10; i++)
	{
		read(sig_fd, r_buf, sizeof(r_buf));
		//jni_dbg("<jni> recv 4G signal: %s\n", r_buf);
		usleep(100);

		p = strstr(r_buf, "+CSQ:");
		if(p != NULL) {
			p = strrchr(r_buf, ':');
			sprintf(r_buf, "%s", p + 2);
			//jni_dbg("read_buf:%s\n", r_buf);

			p = strchr(r_buf, ',');
			*p = '\0';
			//jni_dbg("read_buf:%s\n", r_buf);
			signal = atoi(r_buf);
			jni_dbg("<jni> 4g signal: %d\n", signal);
			close(sig_fd);
			return signal;
		}
	}

	close(sig_fd);
	return -1;	
}


/*
 * Class:     com_zjpavt_client_nativemethod_JnIMethod
 * Method:    restart4GNative
 * Signature: ()V
 * 重启4G模块
 */
JNIEXPORT void JNICALL Java_com_zjpavt_client_nativemethod_JnIMethod_restart4GNative
  (JNIEnv *env, jclass obj)
{
	int fd;
	
	system("/usr/bin/killall pppd");

	fd = open("/dev/other_gpio0", O_RDWR);
	if(fd < 0){
		perror("open dev error!\n");
		return ;
	}

	ioctl(fd,  IOREQ_CARDA_1_OFF);
	sleep(10);
	ioctl(fd,  IOREQ_CARDA_1_EN);
	sleep(10);

	system("/usr/sbin/pppd call gprs&");
	sleep(10);
	return ;
}

