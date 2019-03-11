/*************************************************************************
    > File Name: other_gpio.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年09月04日 星期一 17时33分03秒
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

#include "other_gpio.h"


#define BUFSIZE 100
#define OTHER_GPIO_NAME  "other_gpio"
#define OTHER_GPIO_MAJOR  202

struct other_gpio_dev{
	struct cdev dev;
	dev_t devno;
};
static struct other_gpio_dev *g_other_gpio_devp;

struct class *g_other_gpio_class;

static int other_gpio_request(void)
{
	int ret = 0;

	printk("*** other_gpio_request ... ***\n");
	ret = gpio_request(BUZZER, "buzzer");
	if(ret){
		printk("request buzzer gpio  error!\n");
		return ret;
	}

	ret = gpio_request(RF_PWR, "rf_pwr");
	if(ret){
		printk("request rf power gpio  error!\n");
		return ret;
	}
	ret = gpio_request(RF_RST, "rf_rst");
	if(ret){
		printk("request rf rst gpio  error!\n");
		return ret;
	}

	ret = gpio_request(LTE_RST, "lte_rst");
	if(ret){
		printk("request lte rst gpio  error!\n");
		return ret;
	}
	ret = gpio_request(LTE_PWR, "lte_pwr");
	if(ret){
		printk("request lte power gpio  error!\n");
		return ret;
	}

	ret = gpio_request(FIBER_PWR, "fiber_pwr");
	if(ret){
		printk("request fiber power gpio  error!\n");
		return ret;
	}

	gpio_direction_output(BUZZER, 0);
	gpio_direction_output(RF_PWR, 1);
	gpio_direction_output(RF_RST, 0);
	gpio_direction_output(LTE_PWR, 1);
	gpio_direction_output(LTE_RST, 0);
	gpio_direction_output(FIBER_PWR, 0);

	return 0;
}

static int other_gpio_open(struct inode *inode, struct file *filp)
{
	printk("*** <kernel> other_gpio_open *** \n");
	return 0;
}
static int other_gpio_release(struct inode *inode, struct file *filp)
{
	//int ret = 0;

	return 0;
}

static ssize_t other_gpio_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	return 0;	
}

static ssize_t other_gpio_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;	
}

static long other_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	//printk("*** <kenrel> other_gpio_ioctl *** \n");
	if(_IOC_TYPE(cmd) != CARD_MAJIC){
		return -EINVAL;
	}

	if(_IOC_NR(cmd)>0xa7 || _IOC_NR(cmd)<0xa0){
		return -EINVAL;
	}

	switch(cmd)
	{
		case IOREQ_BUZZER_ON:
		gpio_set_value(BUZZER, 1);
		break;

		case IOREQ_BUZZER_OFF:
		gpio_set_value(BUZZER, 0);
		break;

		case IOREQ_RF_PWR_ON:
		gpio_set_value(RF_PWR, 1);
		break;	

		case IOREQ_RF_PWR_OFF:
		gpio_set_value(RF_PWR, 0);
		break;	

		case IOREQ_LTE_PWR_ON:
		gpio_set_value(LTE_PWR, 1);
		break;	

		case IOREQ_LTE_PWR_OFF:
		gpio_set_value(LTE_PWR, 0);
		break;	

		default:
		printk("no such opration!\n");
		return -1;
	}
		return 0;
}


static struct file_operations other_gpio_fops = {
	.owner = THIS_MODULE,
	.open = other_gpio_open,
	.read = other_gpio_read,
	.write = other_gpio_write,
	.release = other_gpio_release,
	.unlocked_ioctl = other_gpio_ioctl,
};

static int gpio_card_probe(struct platform_device *pdev)
{
	int ret = 0;
	
	printk("**** card gpio driver! ****\n");

	g_other_gpio_devp = kmalloc(sizeof(struct other_gpio_dev), GFP_KERNEL);
	if(!g_other_gpio_devp){
		printk("card gpio kmalloc fail!\n");
		ret = -ENOMEM;
		goto fail_malloc;
	}

	g_other_gpio_devp->devno = MKDEV(OTHER_GPIO_MAJOR, 0);
	ret = register_chrdev_region(g_other_gpio_devp->devno, 1, OTHER_GPIO_NAME);
	if(ret){
		printk("other_gpio:can`t get major %d\n", OTHER_GPIO_MAJOR);
		return ret;
	}
	
	cdev_init(&g_other_gpio_devp->dev, &other_gpio_fops);
	g_other_gpio_devp->dev.owner = THIS_MODULE;
	g_other_gpio_devp->dev.ops = &other_gpio_fops;
	ret = cdev_add(&g_other_gpio_devp->dev, g_other_gpio_devp->devno, 1);
	if(ret){
		printk("gpio_card: adding dev error!\n");
		return ret;
	}
	g_other_gpio_class = class_create(THIS_MODULE, "other_gpio_class");
	device_create(g_other_gpio_class, NULL, g_other_gpio_devp->devno, OTHER_GPIO_NAME, "other_gpio%d", 0);

	other_gpio_request();
	printk("**** card gpio initialized. ****\n");

	return ret;

fail_malloc:
	kfree(g_other_gpio_devp);
	return -1;
}

static int gpio_card_remove(struct platform_device *pdev)
{
	gpio_free(BUZZER);
	gpio_free(RF_PWR);
	gpio_free(RF_RST);
	gpio_free(LTE_PWR);
	gpio_free(LTE_RST);
	gpio_free(FIBER_PWR);
	device_destroy(g_other_gpio_class, g_other_gpio_devp->devno);
	class_destroy(g_other_gpio_class);
	cdev_del(&g_other_gpio_devp->dev);
	unregister_chrdev_region(g_other_gpio_devp->devno, 1);
	kfree(g_other_gpio_devp);

	return 0;
}

static struct of_device_id gpio_card_of_match[] = {
		{ .compatible = "gpio-card", },
		{},
};
MODULE_DEVICE_TABLE(of, gpio_card_of_match);

static struct platform_driver gpio_card_device_driver = {
	.probe		= gpio_card_probe,
	.remove		= gpio_card_remove,
	.driver		= {
		.name	= "gpio-card",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(gpio_card_of_match),
	}
};

static int __init gpio_card_init(void)
{
		return platform_driver_register(&gpio_card_device_driver);
}

static void __exit gpio_card_exit(void)
{
		platform_driver_unregister(&gpio_card_device_driver);
}

late_initcall(gpio_card_init);
module_exit(gpio_card_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PAVT Star");
MODULE_DESCRIPTION("GPIO_card driver for GPIOs");
MODULE_ALIAS("platform:gpio-keys");
