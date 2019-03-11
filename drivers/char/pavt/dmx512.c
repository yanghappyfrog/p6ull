/*************************************************************************
    > File Name: dmx512.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年12月27日 星期一 17时33分03秒
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

#include "dmx512.h"


#define BUFSIZE 100
#define dmx512_GPIO_NAME  "dmx512_gpio"
#define dmx512_GPIO_MAJOR  203

struct dmx512_gpio_dev{
	struct cdev dev;
	dev_t devno;
};
static struct dmx512_gpio_dev *g_dmx512_gpio_devp;

struct class *g_dmx512_gpio_class;

static int dmx512_gpio_request(void)
{
	int ret = 0;

	printk("*** dmx512_gpio_request ... ***\n");
	ret = gpio_request(DMX_TX, "dmx-tx");
	if(ret){
		printk("request gpio dmx-tx  error!\n");
		return ret;
	}
	ret = gpio_request(DMX_RX, "dmx-rx");
	if(ret){
		printk("request gpio dmx-rx  error!\n");
		return ret;
	}

	gpio_direction_output(DMX_TX, 1);
	gpio_direction_output(DMX_RX, 0);

	return 0;
}

static int dmx512_gpio_open(struct inode *inode, struct file *filp)
{
	printk("*** <kernel> dmx512_gpio_open *** \n");
	return 0;
}
static int dmx512_gpio_release(struct inode *inode, struct file *filp)
{
	//int ret = 0;

	return 0;
}

static ssize_t dmx512_gpio_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	return 0;	
}

static ssize_t dmx512_gpio_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;	
}

static long dmx512_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	//printk("*** <kenrel> dmx512_gpio_ioctl *** \n");
	if(_IOC_TYPE(cmd) != CARD_MAJIC){
		return -EINVAL;
	}

	if(_IOC_NR(cmd)>0xa7 || _IOC_NR(cmd)<0xa0){
		return -EINVAL;
	}

	switch(cmd)
	{
		case IOREQ_DMX_TX_H:
		gpio_set_value(DMX_TX, 1);
		break;

		case IOREQ_DMX_TX_L:
		gpio_set_value(DMX_TX, 0);
		break;

		default:
		printk("no such opration!\n");
		return -1;
	}
		return 0;
}


static struct file_operations dmx512_gpio_fops = {
	.owner = THIS_MODULE,
	.open = dmx512_gpio_open,
	.read = dmx512_gpio_read,
	.write = dmx512_gpio_write,
	.release = dmx512_gpio_release,
	.unlocked_ioctl = dmx512_gpio_ioctl,
};

static int gpio_dmx512_probe(struct platform_device *pdev)
{
	int ret = 0;
	
	printk("**** dmx512 gpio driver! ****\n");

	g_dmx512_gpio_devp = kmalloc(sizeof(struct dmx512_gpio_dev), GFP_KERNEL);
	if(!g_dmx512_gpio_devp){
		printk("card gpio kmalloc fail!\n");
		ret = -ENOMEM;
		goto fail_malloc;
	}

	g_dmx512_gpio_devp->devno = MKDEV(dmx512_GPIO_MAJOR, 0);
	ret = register_chrdev_region(g_dmx512_gpio_devp->devno, 1, dmx512_GPIO_NAME);
	if(ret){
		printk("dmx512_gpio:can`t get major %d\n", dmx512_GPIO_MAJOR);
		return ret;
	}
	
	cdev_init(&g_dmx512_gpio_devp->dev, &dmx512_gpio_fops);
	g_dmx512_gpio_devp->dev.owner = THIS_MODULE;
	g_dmx512_gpio_devp->dev.ops = &dmx512_gpio_fops;
	ret = cdev_add(&g_dmx512_gpio_devp->dev, g_dmx512_gpio_devp->devno, 1);
	if(ret){
		printk("gpio_card: adding dev error!\n");
		return ret;
	}
	g_dmx512_gpio_class = class_create(THIS_MODULE, "dmx512_gpio_class");
	device_create(g_dmx512_gpio_class, NULL, g_dmx512_gpio_devp->devno, dmx512_GPIO_NAME, "dmx512_gpio%d", 0);

	dmx512_gpio_request();
	printk("**** dmx512 gpio initialized. ****\n");

	return ret;

fail_malloc:
	kfree(g_dmx512_gpio_devp);
	return -1;
}

static int gpio_dmx512_remove(struct platform_device *pdev)
{
	gpio_free(DMX_TX);
	gpio_free(DMX_RX);
	device_destroy(g_dmx512_gpio_class, g_dmx512_gpio_devp->devno);
	class_destroy(g_dmx512_gpio_class);
	cdev_del(&g_dmx512_gpio_devp->dev);
	unregister_chrdev_region(g_dmx512_gpio_devp->devno, 1);
	kfree(g_dmx512_gpio_devp);

	return 0;
}

static struct of_device_id gpio_dmx512_of_match[] = {
		{ .compatible = "gpio-dmx512", },
		{},
};
MODULE_DEVICE_TABLE(of, gpio_dmx512_of_match);

static struct platform_driver gpio_dmx512_device_driver = {
	.probe		= gpio_dmx512_probe,
	.remove		= gpio_dmx512_remove,
	.driver		= {
		.name	= "gpio-dmx512",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(gpio_dmx512_of_match),
	}
};

static int __init gpio_dmx512_init(void)
{
		return platform_driver_register(&gpio_dmx512_device_driver);
}

static void __exit gpio_dmx512_exit(void)
{
		platform_driver_unregister(&gpio_dmx512_device_driver);
}

late_initcall(gpio_dmx512_init);
module_exit(gpio_dmx512_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PAVT Star");
MODULE_DESCRIPTION("GPIO_DMX512 driver for GPIOs");
MODULE_ALIAS("platform:gpio-dmx512");
