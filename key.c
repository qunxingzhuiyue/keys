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
#include <time.h>

static int Key_Major;

static struct class *Keydrv_class;
static struct class_device *Key_drv_dev;
typedef struct key_struct
{
	int key_number;
	struct timeval key_pressed_time;
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
	struct timeval last;
	unsign int key_val;
	int err;
	
	key_id=(struct key_struct *)dev_id; 
	err=do_gettimeofday(&last);
	if(s3c2410_gpio_getpin(*key_id->pin)) //key的值为高,上升沿,按键弹起
	{
		if(err>=0)
		{
			if(((*key_id->key_pressed_time)!=0x0)&&(30<key_val=((((last->tv_sec)-(*key_id->key_pressed_time->tv_sec))*1000+((last->tv_usec)-(*key_id->key_pressed_time->tv_usec))))))
			{
				printk("key %d press time: %d ms\n\r" , *key_id->key_number , key_val);
				*key_id->key_pressed_time->tv_sec=0;
				*key_id->key_pressed_time->tv_usec=0;
				switch (key_id->key_number)
				{
					case 1: //key 1 handler
					{
						
						break;
					}
					case 2:
					{
						break;
					}
					case 3:
					{
						break;
					}
					case 4:
					{
						break;
					}
				}
			}
			else      //less then 30ms key , invalid
				printk("key press invalid\n\r")
		}
		else
			printk("read key press time error!\n\r");
	}
	else     //下降沿，按键按下，开始计时
	{
		if(err>=0)
		{
			*key_id->key_pressed_time->tv_sec=last->tv_sec;
			*key_id->key_pressed_time->tv_usec=last->tv_usec;
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
	
	free_irq(IRQ_EINT0  , key_list[0]);
	free_irq(IRQ_EINT2  , key_list[1]);
	free_irq(IRQ_EINT11 , key_list[2]);
	free_irq(IRQ_EINT19 , key_list[3]);
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



