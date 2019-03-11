CROSS_COMPILE = arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

.PHONY:all
all:uart_gpio

uart_gpio:uart_gpio.o
uart_gpio.o:uart_gpio.c
	$(CC) -c $(CFLAGS)  uart_gpio.c -o uart_gpio.o


.PHONY:clean

clean:
	rm -f *~ *.o
	rm uart_gpio


