CROSS_COMPILE = arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

.PHONY:all
all:other_gpio

other_gpio:other_gpio.o
other_gpio.o:other_gpio.c
	$(CC) -c $(CFLAGS)  other_gpio.c -o other_gpio.o


.PHONY:clean

clean:
	rm -f *~ *.o
	rm other_gpio


