CROSS_COMPLER = arm-linux-gnueabihf-
CC := $(CROSS_COMPLER)gcc
LD := $(CROSS_COMPLER)ld

.PHONY:all
all:airsw_test

airsw_test:airsw_test.o
serial:serial.o
airsw_test.o:airsw_test.c
	$(CC) -c $(CFLAGS)  airsw_test.c -o airsw_test.o serial.o


.PHONY:clean

clean:
	rm -f *~ *.o
	rm airsw_test


