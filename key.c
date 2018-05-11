/*This is the key driver for s3c2440*/



#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

static int Key_Major;

static struct class *Keydrv_class;
static struct class_device *Key_drv_dev;
typedef struct key_struct
{
	int key_number;
	int key_pressed_time;
	int key_status;
	int pin;
}

static struct key_struct key_list[4]={
	{1,0,0,S3C2410_GPF0},
	{2,0,0,S3C2410_GPF2},
	{3,0,0,S3C2410_GPG3},
	{4,0,0,S3C2410_GPG11},
};

static irqreturn_t Key_irqhandler(int irq, void *dev_id)
{
	sturct key_struct *key_id;
	key_id=(struct key_struct *)dev_id; 
	
	switch (key_id->key_number)
	{
		case 1: //key 1 handler
		{
			
		}
	}
	return 0;
}




static int Key_open(struct inode *inode, struct file *file)
{
	//可以加入错误校验机制，如打印等
	int err;
	err=request_irq(IRQ_EINT0  , Key_irqhandler , IRQT_BOTHEDGE ,"K1", key_list[0] );
	if(err<0)
		printk("K1 init err err No. is : %d",err);
	err=request_irq(IRQ_EINT2  , Key_irqhandler , IRQT_BOTHEDGE ,"K2", key_list[1] );
	if(err<0)
		printk("K2 init err err No. is : %d",err);
	err=request_irq(IRQ_EINT11 , Key_irqhandler , IRQT_BOTHEDGE ,"K3", key_list[2] );
	if(err<0)
		printk("K3 init err err No. is : %d",err);
	err=request_irq(IRQ_EINT19 , Key_irqhandler , IRQT_BOTHEDGE ,"K4", key_list[3] );
	if(err<0)
		printk("K4 init err err No. is : %d",err);
	return 0;
}




static int Key_READ(struct file *file, char __user *buf, size_t len, loff_t *loff_t)
{
	
	return 0;
}

int Key_irq_closed(struct inode *inode, struct file *file)
{
	
	free_irq(IRQ_EINT0  , 1);
	free_irq(IRQ_EINT2  , 2);
	free_irq(IRQ_EINT11 , 3);
	free_irq(IRQ_EINT19 , 4);
	return 0;
}



static struct file_operations Key_drv_fops={
	.owner=THIS_MODULE,
	.read=KEY_READ,
	.open=Key_open,	
	.release=Key_irq_closed;
};



int Key_drv_init()
{
	Key_Major=register_chrdev(0 , "Keys" , &Key_drv_fops);
	if(Key_Major<0)
	{
		printk("fail to add device key!\n\r");
		return Key_Major;
	}
	Keydrv_class=class_create(THIS_MODULE , "Keys");
	if(unlikely(IS_ERR(Keydrv_class)))
	{
		printk("fail to creat class!\n\r");
		return PTR_ERR(Keydrv_class);
	}
	Key_drv_dev=class_device_create(Keydrv_class , NULL , MKDEV(Key_Major,0), Null , "Keys");
	if(unlikely(IS_ERR(Key_drv_dev)))
	{
		printk("fail to creat device!\n\r");
		return PTR_ERR(Key_drv_dev);
	}
}


int Key_drv_exit()
{
	unregister_chrdev(Key_Major , "Keys");
	class_device_unregister(Key_drv_dev);
	class_destroy(Keydrv_class);
}



module_init(Key_drv_init);
module_exit(Key_drv_exit);


MODULE_LICENSE("GPL");



