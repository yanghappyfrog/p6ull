/*************************************************************************
    > File Name: ext1.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年12月19日 星期三 14时11分38秒
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/rtc.h>
#include <linux/time.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <asm/signal.h>
#include <asm-generic/siginfo.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/gpio.h>

#define BUFSIZE 64
#define EXT1_NAME "ext1"
#define EXT1_MAJOR 204

/* Convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio)	(32 * (bank - 1) + (gpio))   //IMX6

struct ext1_dev{
	struct cdev dev;
	dev_t devno;
	int irq;
	struct fasync_struct *fasync;
};
static struct ext1_dev *g_ext1_devp;
struct class *g_ext1_class;

static int ext1_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, &g_ext1_devp->fasync);
}

static irqreturn_t ext1_irq_handle(int irqno, void *dev_id)
{
	if(g_ext1_devp->fasync)
		kill_fasync(&g_ext1_devp->fasync, SIGIO, POLL_IN);

	return IRQ_HANDLED;
}

static int ext1_open(struct inode *inode, struct file *filp)
{
	try_module_get(THIS_MODULE);
	return 0;
}

static int ext1_release(struct inode *inode, struct file *filp)
{
	ext1_fasync(-1, filp, 0);
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations ext1_fops = {
	.owner = THIS_MODULE,
	.open = ext1_open,
	.release = ext1_release,
	.fasync = ext1_fasync,
};

static int ext1_probe(struct platform_device *pdev)
{
	int ret = 0;

	printk("******** ext1 interrupt driver !\n********");

	g_ext1_devp = kmalloc(sizeof(struct ext1_dev), GFP_KERNEL);
	if(!g_ext1_devp){
		printk("ext1 kmalloc fail!\n");
		ret = -ENOMEM;
		goto fail_malloc;
	}

	g_ext1_devp->devno = MKDEV(EXT1_MAJOR, 0);
	ret = register_chrdev_region(g_ext1_devp->devno, 1, EXT1_NAME);
	if(ret){
		printk("get ext1 major error!\n");
		goto fail_malloc;
	}

	cdev_init(&g_ext1_devp->dev, &ext1_fops);
	g_ext1_devp->dev.owner = THIS_MODULE;
	g_ext1_devp->dev.ops = &ext1_fops;
	ret = cdev_add(&g_ext1_devp->dev, g_ext1_devp->devno, 1);
	if(ret){
		printk("add ext1 dev error!\n");
		goto fail_malloc;
	}
	g_ext1_class = class_create(THIS_MODULE, "ext1_class");
	device_create(g_ext1_class, NULL, g_ext1_devp->devno, EXT1_NAME, "ext1_%d", 0);

	g_ext1_devp->irq = gpio_to_irq(GPIO_TO_PIN(5, 1));	//snvs_tamper1
	ret = request_irq(g_ext1_devp->irq, ext1_irq_handle, 
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "ext1_irq", g_ext1_devp);
	if(ret){
		printk("request ext1 irq error!\n");
		goto fail_malloc;
	}

	printk("*********** ext1 interrupt driver initialized. ***********\n");
	return 0;

fail_malloc:
	kfree(g_ext1_devp);
	return ret;
}

static int ext1_remove(struct platform_device *pdev)
{
	free_irq(g_ext1_devp->irq, g_ext1_devp);
	device_destroy(g_ext1_class, g_ext1_devp->devno);
	class_destroy(g_ext1_class);
	cdev_del(&g_ext1_devp->dev);
	unregister_chrdev_region(g_ext1_devp->devno, 1);
	kfree(g_ext1_devp);
	return 0;
}


static struct of_device_id ext1_of_match[] = {
	{.compatible = "ext1",},
	{},
};

static struct platform_driver ext1_device_driver = {
	.probe = ext1_probe,
	.remove = ext1_remove,
	.driver = {
		.name = "ext1",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ext1_of_match),
	}
};

static int __init ext1_init(void)
{
	return platform_driver_register(&ext1_device_driver);
}

static void __exit ext1_exit(void)
{
	return platform_driver_unregister(&ext1_device_driver);
}

late_initcall(ext1_init);
module_exit(ext1_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for ex_com1 interrupt!");
MODULE_AUTHOR("PAVT Happyfrog 2018.12.19");

