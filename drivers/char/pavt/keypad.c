/*************************************************************************
    > File Name: keypad.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2017年06月21日 星期三 14时14分44秒
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

#include "keypad.h"

#define BUFSIZE 64
#define KEYPAD_NAME "keypad"
#define KEYPAD_MAJOR 200

/* Convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio)	(32 * (bank - 1) + (gpio))   //IMX6

struct keypad_dev{
	struct cdev dev;
	dev_t devno;
	int irq;
	struct fasync_struct *fasync;
};
static struct keypad_dev *g_keypad_devp;
struct class *g_keypad_class;

struct tasklet_struct g_keypad_tasklet;
struct i2c_client *keypad_client;

static int keypad_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, &g_keypad_devp->fasync);	
}

static void keypad_irq_tasklet(unsigned long data)
{
	char key_val = 0xff;
	//printk("@@@ <kernel> keypad_irq_tasklet!\n");
}

static irqreturn_t keypad_irq_handle(int irqno, void *dev_id)
{
	//printk("@@@ <kernel> keypad_irq_handle!\n");
	//tasklet_schedule(&g_keypad_tasklet);
	 if (g_keypad_devp->fasync)  
		 kill_fasync(&g_keypad_devp->fasync, SIGIO, POLL_IN);  

	return IRQ_HANDLED;
}

static int keypad_open(struct inode *inode, struct file *filp)
{
	int ret;

	printk("@@@ <kernel> keypad_open...\n");
	
	//ret = request_irq(g_keypad_devp->irq, keypad_irq_handle, IRQF_SHARED | IRQF_TRIGGER_FALLING, "keypad_irq", g_keypad_devp);
	//if(ret){
	//	printk("request keypad irq error!\n");
	//	goto err_exit;
	//}
	//irq_set_irq_type(g_keypad_devp->irq, IRQ_TYPE_EDGE_FALLING);
	//irq_set_irq_type(g_keypad_devp->irq, IRQF_TRIGGER_FALLING);

	try_module_get(THIS_MODULE);
	return 0;

err_exit:
	return -1;
}

static int keypad_release(struct inode *inode, struct file *filp)
{
	keypad_fasync(-1, filp, 0);
	module_put(THIS_MODULE);
	return 0;
}

static int keypad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	char ret;

	//printk("********** keypad read! *************\n");
	ret = i2c_smbus_read_byte_data(keypad_client, 0x0); //key value address: 0x0;
	if(ret < 0){
		printk("<kernel> read key value error!\n");
		return -1;
	}
	//printk("<kernel> key value: 0x%x\n", ret);

	ret = copy_to_user(buff, &ret, sizeof(ret));
	return 0;
}

static int keypad_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	int ret; 
	char wbuf[BUFSIZE];
	long addr;

	u8 w_buf[3] = {3, 2, 0x33}; 
	struct i2c_msg msgs;

	//printk("********** keypad write! *************\n");
	ret = copy_from_user(wbuf, buff, BUFSIZE);
	if(ret){
		printk("copy_from_user error!\n");
	}
	//printk("**** <kernel> keypad wbuf: [%s] ****\n", wbuf);

	msgs.flags = 0;
	msgs.addr = 0x2;  //x,y,c
	msgs.len = 3;
	msgs.buf = w_buf;
	i2c_transfer(keypad_client->adapter, &msgs, 1);

	return 0;
}

static long keypad_ioctl(struct file *filp, unsigned int cmd,  unsigned long arg)
{
	unsigned long ret = 0;

	if(_IOC_TYPE(cmd) != PAVT_KEYPAD_MAJIC){
		return -EINVAL;
	}
	if(_IOC_NR(cmd)>0xc0 || _IOC_NR(cmd)<0xa0){
		return -EINVAL;
	}

	switch(cmd)
	{
		case WRITE_CHAR:
			break;

		default:
			printk("no such opration!\n");
			return -1;
	}

	return 0;
}

static struct file_operations keypad_fops = {
	.owner = THIS_MODULE, 
	.open = keypad_open,
	.read = keypad_read,
	.write = keypad_write,
	.release = keypad_release,
	.unlocked_ioctl = keypad_ioctl,
	.fasync = keypad_fasync,
};

static int keypad_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;

	printk(" *** keypad_probe... ***\n");
	if (!i2c_check_functionality(client->adapter,
				I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
		return -EIO;

	keypad_client = client;
	printk("@@@ keypad_client->addr: 0x%x\n", keypad_client->addr);

	g_keypad_devp = kmalloc(sizeof(struct keypad_dev), GFP_KERNEL);
	if(!g_keypad_devp){
		printk(" keypad malloc failed!!!\n");
		return -ENOMEM;
	}
	memset(g_keypad_devp, 0, sizeof(struct keypad_dev));

	g_keypad_devp->devno = MKDEV(KEYPAD_MAJOR, 0);
	ret = register_chrdev_region(g_keypad_devp->devno, 1, KEYPAD_NAME);
	if(ret){
		printk("register char device failed!!!\n");
		return ret;
	}

	cdev_init(&g_keypad_devp->dev, &keypad_fops);
	g_keypad_devp->dev.owner = THIS_MODULE;
	g_keypad_devp->dev.ops = &keypad_fops;

	ret = cdev_add(&g_keypad_devp->dev, g_keypad_devp->devno, 1);
	if(ret){
		printk(" add keypad dev error !!!\n");
		return -1;
	}
	g_keypad_class = class_create(THIS_MODULE, "keypad_class");
	device_create(g_keypad_class, NULL, g_keypad_devp->devno, KEYPAD_NAME, "keypad%d", 0);

	g_keypad_devp->irq = gpio_to_irq(GPIO_TO_PIN(4,19));		//keypad INT CSI_VSYNC__GPIO4_19
	printk("@@@ <kenrel> keypad irq: %d\n", g_keypad_devp->irq);
	ret = request_irq(g_keypad_devp->irq, keypad_irq_handle, IRQF_SHARED | IRQF_TRIGGER_FALLING, "keypad_irq", g_keypad_devp);
	if(ret){
		printk("request keypad irq error!\n");
	}

	tasklet_init(&g_keypad_tasklet, keypad_irq_tasklet, (unsigned long)g_keypad_devp);


	return 0;
}

static int keypad_remove(struct i2c_client *client)
{
	printk(" *** keypad_remove... ***\n");

	free_irq(g_keypad_devp->irq, g_keypad_devp);
	device_destroy(g_keypad_class, g_keypad_devp->devno);
	class_destroy(g_keypad_class);
	cdev_del(&g_keypad_devp->dev);
	unregister_chrdev_region(g_keypad_devp->devno, 1);
	kfree(g_keypad_devp);
	return 0;
}

static const struct of_device_id keypad_match[] = {
	{.compatible = "keypad",},
	{},
};
MODULE_DEVICE_TABLE(of, keypad_match);

static const struct i2c_device_id keypad_ids[] = {
	{"keypad", 0},
	{ },
};

static struct i2c_driver keypad_driver = {
	.driver = {
		.name    = "keypad",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(keypad_match),
	},
	.probe = keypad_probe,
	.remove = keypad_remove,
	.id_table = keypad_ids,
};

static int keypad_module_init(void)
{
	i2c_add_driver(&keypad_driver);
	return 0;
}

static void keypad_module_exit(void)
{
	i2c_del_driver(&keypad_driver);
}

module_init(keypad_module_init);
module_exit(keypad_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for keypad");
MODULE_AUTHOR("PAVT Happyfrog 2017.6.21");


