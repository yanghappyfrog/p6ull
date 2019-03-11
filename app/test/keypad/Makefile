CROSS_COMPLER = arm-linux-gnueabihf-
CC := $(CROSS_COMPLER)gcc
LD := $(CROSS_COMPLER)ld

.PHONY:all
all:keypad_test

keypad_test:keypad_test.o
keypad_test.o:keypad_test.c
	$(CC) -c $(CFLAGS)  keypad_test.c -o keypad_test.o


.PHONY:clean

clean:
	rm -f *~ *.o
	rm keypad_test


