#for x86 test
#gcc -fPIC -shared -o libpavtjni.so pavt_jni.c serial.c -I/usr/local/java/jdk1.8.0_151/include -I/usr/local/java/jdk1.8.0_151/include/linux

#for arm
/home/star/work/tools/toolchains/gcc-linaro-arm-linux-gnueabihf-4.9-2014.07_linux/bin/arm-linux-gnueabihf-gcc \
		-fPIC -shared -o libpavtjni.so pavt_jni.c scan_exuart.c updateinfo.c serial.c inifile.c dictionary.c iniparser.c\
		-I/usr/local/java/jdk1.8.0_191/include/ -I/usr/local/java/jdk1.8.0_191/include/linux 

