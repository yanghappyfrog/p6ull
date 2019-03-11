#########################################################################
# File Name: mk.sh
# Author: Happyfrog
# mail: yanghappyfrog@163.com
# Created Time: 2017年04月06日 星期四 11时41分26秒
#########################################################################
#!/bin/bash
source /home/star/work/tools/toolchains/meta-toolchain/environment-setup-cortexa7hf-neon-poky-linux-gnueabi

if [ $1 = "dtb" ]
	then
	echo " Make dtbs... "
	make  ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- myd-y6ull-gpmi-weim.dtb 
	cp arch/arm/boot/dts/myd-y6ull-gpmi-weim.dtb ~/windows/d_fs/tftp/p6ull/
	elif [ $1 = "zImage" ]
	then
	echo " Make zImage... "
	make  ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- zImage
	cp arch/arm/boot/zImage ~/windows/d_fs/tftp/p6ull/
	elif [ $1 = "def" ]
	then
	echo " Make def_config... "
	make  ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- myd_y6ulx_defconfig
	else
	echo " para error... "
	echo " make dtb "
	echo " make zImage "
	echo " make def "
fi

