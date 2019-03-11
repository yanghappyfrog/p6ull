#########################################################################
# File Name: 4g.sh
# Author: Happyfrog
# mail: yanghappyfrog@163.com
# Created Time: 2018年07月02日 星期四 10时00分26秒
#########################################################################
#!/bin/bash

if [ $1 = "4g" ]
	then
	echo " change to 4g ... "
	route del default
	route add default dev ppp0
	echo "nameserver 114.114.114.114" > /etc/resolv.conf
	elif [ $1 = "eth0" ]
	then
	echo " change to eth0 ... "
	route del default
	route add default dev eth0
	udhcpc 
	elif [ $1 = "eth1" ]
	then
	echo " change to eth1 ... "
	route del default
	route add default dev eth1
	udhcpc 
	else
	echo " para error... "
fi

