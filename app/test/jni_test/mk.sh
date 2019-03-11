#for x86 test
#gcc -fPIC -shared -o libTestJni.so TestJni.c serial.c -I/usr/local/java/jdk1.8.0_151/include -I/usr/local/java/jdk1.8.0_151/include/linux

#for arm
/home/star/work/tools/toolchains/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-gcc \
		-fPIC -shared -o libTestJni.so TestJni.c serial.c inifile.c dictionary.c iniparser.c\
		-I/usr/local/java/jdk1.8.0_151/include -I/usr/local/java/jdk1.8.0_151/include/linux 

