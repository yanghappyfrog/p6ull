/*************************************************************************
    > File Name: uart_gpio.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年06月07日 星期三 18时43分03秒
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm-generic/ioctl.h>
#include <linux/bcd.h>

#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include "uart_gpio.h"

#define UART_ADDR0	(32*(1-1)+11)  //GPIO1_11
#define UART_ADDR1	(32*(1-1)+12)  //GPIO1_12
#define UART_ADDR2	(32*(1-1)+14)  //GPIO1_14	
#define UART_ADDR3	(32*(1-1)+13)  //GPIO1_13

#define BUFSIZE 100
#define UART_GPIO_NAME  "uart_gpio"
#define UART_GPIO_MAJOR  201

struct uart_gpio_dev{
	struct cdev dev;
	dev_t devno;
};
static struct uart_gpio_dev *g_uart_gpio_devp;

struct class *g_uart_gpio_class;

static int uart_gpio_requset(void)
{
	int ret = 0;

	printk("*** uart_gpio_open ... ***\n");
	ret = gpio_request(UART_ADDR0, "addr0");
	if(ret){
		printk("request gpio addr0 error!\n");
		return ret;
	}
	ret = gpio_request(UART_ADDR1, "addr1");
	if(ret){
		printk("request gpio addr1 error!\n");
		return ret;
	}
	ret = gpio_request(UART_ADDR2, "addr2");
	if(ret){
		printk("request gpio addr2 error!\n");
		return ret;
	}
	ret = gpio_request(UART_ADDR3, "addr3");
	if(ret){
		printk("request gpio addr3 error!\n");
		return ret;
	}
	gpio_direction_output(UART_ADDR0, 0);
	gpio_direction_output(UART_ADDR1, 0);
	gpio_direction_output(UART_ADDR2, 0);
	gpio_direction_output(UART_ADDR3, 0);

	return 0;
}

static int uart_gpio_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int uart_gpio_release(struct inode *inode, struct file *filp)
{
	//int ret = 0;

	return 0;

}

static ssize_t uart_gpio_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
//	int ret;
//	char data;
//
//	printk("*** uart_gpio_read ... ***\n");
//	data = gpio_get_value(23);
//	printk("*** data: Ox%x ***\n", data);
//
//	ret = copy_to_user(data, buff, BUFSIZE);
//	if(ret){
//		printk(" copy_to_user error!\n");
//	}

	return 0;	
}

static ssize_t uart_gpio_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	int ret;
	char wbuf[BUFSIZE];
	long addr = 0;

	//printk("*** uart_gpio_write ... ***\n");
	ret = copy_from_user(wbuf, buff, BUFSIZE);
	if(ret){
		printk(" copy_from_user error!\n");
	}
	//printk("**** <kernel> uart gpio: wbuf[%s] ****\n", wbuf);

	addr = simple_strtol(wbuf, NULL, 0);
	addr = bcd2bin(addr);

	//printk("*** <kernel> bit[0]: %ld\n", (addr & 0x1));
	//printk("*** <kernel> bit[1]: %ld\n", (addr & 0x2) >> 1);
	//printk("*** <kernel> bit[2]: %ld\n", (addr & 0x4) >> 2);
	//printk("*** <kernel> bit[3]: %ld\n", (addr & 0x8) >> 3);

	gpio_set_value(UART_ADDR0, (addr & 0x1));
	gpio_set_value(UART_ADDR1, (addr & 0x2) >> 1);
	gpio_set_value(UART_ADDR2, (addr & 0x4) >> 2);
	gpio_set_value(UART_ADDR3, (addr & 0x8) >> 3);
	
	return 0;	
}

static long uart_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	if(_IOC_TYPE(cmd) != UART_MAJIC){
		return -EINVAL;
	}

	if(_IOC_NR(cmd)>0xa7 || _IOC_NR(cmd)<0xa0){
		return -EINVAL;
	}

	switch(cmd)
	{

		default:
		printk("no such opration!\n");
		return -1;
	}
		return 0;
}


static struct file_operations uart_gpio_fops = {
	.owner = THIS_MODULE,
	.open = uart_gpio_open,
	.read = uart_gpio_read,
	.write = uart_gpio_write,
	.release = uart_gpio_release,
	.unlocked_ioctl = uart_gpio_ioctl,
};

static int gpio_uart_probe(struct platform_device *pdev)
{
	int ret = 0;
	
	printk("**** uart gpio driver! ****\n");

	g_uart_gpio_devp = kmalloc(sizeof(struct uart_gpio_dev), GFP_KERNEL);
	if(!g_uart_gpio_devp){
		printk("uart gpio kmalloc fail!\n");
		ret = -ENOMEM;
		goto fail_malloc;
	}

	g_uart_gpio_devp->devno = MKDEV(UART_GPIO_MAJOR, 0);
	ret = register_chrdev_region(g_uart_gpio_devp->devno, 1, UART_GPIO_NAME);
	if(ret){
		printk("uart_gpio:can`t get major %d\n", UART_GPIO_MAJOR);
		return ret;
	}
	
	cdev_init(&g_uart_gpio_devp->dev, &uart_gpio_fops);
	g_uart_gpio_devp->dev.owner = THIS_MODULE;
	g_uart_gpio_devp->dev.ops = &uart_gpio_fops;
	ret = cdev_add(&g_uart_gpio_devp->dev, g_uart_gpio_devp->devno, 1);
	if(ret){
		printk("gpio_uart: adding dev error!\n");
		return ret;
	}
	g_uart_gpio_class = class_create(THIS_MODULE, "uart_gpio_class");
	device_create(g_uart_gpio_class, NULL, g_uart_gpio_devp->devno, UART_GPIO_NAME, "uart_gpio%d", 0);

	uart_gpio_requset();

	printk("**** uart gpio initialized. ****\n");

	return ret;

fail_malloc:
	kfree(g_uart_gpio_devp);
	return -1;
}

static int gpio_uart_remove(struct platform_device *pdev)
{
	gpio_free(UART_ADDR0);
	gpio_free(UART_ADDR1);
	gpio_free(UART_ADDR2);
	gpio_free(UART_ADDR3);
	device_destroy(g_uart_gpio_class, g_uart_gpio_devp->devno);
	class_destroy(g_uart_gpio_class);
	cdev_del(&g_uart_gpio_devp->dev);
	unregister_chrdev_region(g_uart_gpio_devp->devno, 1);
	kfree(g_uart_gpio_devp);

	return 0;
}

static struct of_device_id gpio_uart_of_match[] = {
		{ .compatible = "gpio-uart", },
		{},
};
MODULE_DEVICE_TABLE(of, gpio_uart_of_match);

static struct platform_driver gpio_uart_device_driver = {
	.probe		= gpio_uart_probe,
	.remove		= gpio_uart_remove,
	.driver		= {
		.name	= "gpio-uart",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(gpio_uart_of_match),
	}
};

static int __init gpio_uart_init(void)
{
		return platform_driver_register(&gpio_uart_device_driver);
}

static void __exit gpio_uart_exit(void)
{
		platform_driver_unregister(&gpio_uart_device_driver);
}

late_initcall(gpio_uart_init);
module_exit(gpio_uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PAVT Star");
MODULE_DESCRIPTION("GPIO_UART driver for GPIOs");
MODULE_ALIAS("platform:imx6ull");
