#for x86 test
#gcc -fPIC -shared -o libTestJni.so TestJni.c serial.c -I/usr/local/java/jdk1.8.0_151/include -I/usr/local/java/jdk1.8.0_151/include/linux

#for arm
/home/star/work/tools/toolchains/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc \
		-fPIC -shared -o libTestJni.so TestJni.c scan_exuart.c updateinfo.c serial.c inifile.c dictionary.c iniparser.c\
		-I/usr/local/java/jdk1.8.0_191/include/ -I/usr/local/java/jdk1.8.0_191/include/linux 

