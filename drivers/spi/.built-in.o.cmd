cmd_drivers/spi/built-in.o :=  arm-poky-linux-gnueabi-ld -EL    -r -o drivers/spi/built-in.o drivers/spi/spi.o drivers/spi/spi-bitbang.o drivers/spi/spi-gpio.o drivers/spi/spi-imx.o 
