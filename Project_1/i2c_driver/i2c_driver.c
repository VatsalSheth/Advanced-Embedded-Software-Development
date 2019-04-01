/***********************************************************************************
* @i2c_driver.c
* @This file contains character device driver module for I2c.
* 
* @Reference: Linux Kernel i2c-dev.c and linux/i2c.c character device driver code 
* 			  Character device driver basics: https://linux-kernel-labs.github.io/master/labs/device_drivers.html 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define SDA (2)
#define SCL (3)

static int i2c_open(struct inode *inode, struct file *file);
static int i2c_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
static int i2c_write(struct file *file, const char __user *user_buffer, size_t size, loff_t * offset);
static long i2c_ioctl (struct file *file, unsigned int cmd, unsigned long arg);
static int i2c_release(struct inode *inode, struct file *file);
static int i2c_driver_init(void);
static void i2c_driver_exit(void);
/*
void i2c_start(void);
void i2c_stop(void);
void send_ack(uint32_t ack);
uint8_t ack_check(void);
uint8_t ack_check(void);
uint8_t i2c_write_byte(uint8_t value);
uint8_t i2c_read_byte(uint8_t ack);
*/

static int major;
struct class *c;

struct driver_data
{
	uint32_t id;
	uint32_t valid;
	uint8_t slave_addr;
	struct cdev c_dev;
}dev_data[MAX_MINOR_NUM];

const struct file_operations f_op = {
    .owner = THIS_MODULE,
    .open = i2c_open,
    .read = i2c_read,
    .write = i2c_write,
    .release = i2c_release,
    .unlocked_ioctl = i2c_ioctl
};

/*
//start bit logic
void i2c_start()
{
	gpio_direction_output(SDA, 1);
	gpio_set_value(SCL, 0);
	gpio_set_value(SDA, 1);
	gpio_set_value(SCL, 1);
	udelay(2);
	gpio_set_value(SDA, 0);
	udelay(2);
}

//stop bit logic
void i2c_stop()
{
	gpio_direction_output(SDA, 1);
	gpio_set_value(SCL, 0);
	gpio_set_value(SDA, 0);
	gpio_set_value(SCL, 1);
	udelay(2);
	gpio_set_value(SDA, 1);
	udelay(2);
}

//send ack or nack depending on input
//ack=1 sends positive ack
//ack=0 sends negative ack
void send_ack(uint32_t ack)
{
	gpio_direction_output(SDA, 1);
	gpio_set_value(SCL, 0);
	gpio_set_value(SDA, (!ack)&0x01);
	gpio_set_value(SCL, 1);
	udelay(2);
}

//wait for ack to come for 2us which is more then setup time
uint8_t ack_check()
{
	gpio_direction_output(SDA, 1);
	gpio_set_value(SCL, 0);
	gpio_set_value(SDA, 1);
	gpio_direction_output(SDA, 0);
	gpio_set_value(SCL, 1);
	udelay(2);
	if(gpio_get_value(SDA) == 0)
	{
		return 1;
	}
	return 0;
}

//Only data bits send logic and check for ack. Start and stop bits are not part of this function
uint8_t i2c_write_byte(uint8_t value)
{
	int8_t i=0;

	gpio_direction_output(SDA, 1);
	
    for(i=7; i>=0; i--)
    {
		gpio_set_value(SCL, 0);
		gpio_set_value(SDA, (value>>i) & 0x01);
		gpio_set_value(SCL, 1);
		udelay(2);
    }

    if(ack_check()==1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//Reads only 8 bits of data and sends either ack or nack depending on argument.
//ack=1 is positive ack
//ack=0 is negative ack
uint8_t i2c_read_byte(uint8_t ack)
{
    uint8_t i=0,tmp=0;
	
	gpio_direction_output(SDA, 1);
	
    for(i=0; i<8; i++)
    {
		gpio_set_value(SCL, 0);
		gpio_set_value(SDA, 1);
		gpio_direction_output(SDA, 0);
		udelay(2);
		gpio_set_value(SCL, 1);
		udelay(2);
		tmp = (tmp<<1) | (uint8_t)gpio_get_value(SDA);
    }
    send_ack(ack);
    return tmp;
}
*/

static int i2c_open(struct inode *inode, struct file *file)
{
	struct i2c_client *my_client;
	struct i2c_adapter *my_adapter;
	uint32_t minor = iminor(inode);
	
	printk(KERN_INFO "I2C driver open\n");
	
	my_adapter = i2c_get_adapter(minor);
	if (!my_adapter)
		return -ENODEV;
	
	my_client = kmalloc(sizeof(*my_client), GFP_KERNEL);
	if (!my_client) 
	{
		i2c_put_adapter(my_adapter);
		return -ENOMEM;
	}
	
	snprintf(my_client->name, I2C_NAME_SIZE, "i2c_driver %d", my_adapter->nr);

	my_client->adapter = my_adapter;
	file->private_data = my_client;

	return 0;
}

/*
static int i2c_open(struct inode *inode, struct file *file)
{
	struct driver_data *data;
	int i, flag=0;
	
	data = container_of(inode->i_cdev, struct driver_data, c_dev);
	printk(KERN_INFO "pointer %p",data);
	for(i=0; (i<MAX_MINOR_NUM) && (flag==0); i++)
	{
		if(dev_data[i].valid == 1) 
		{
			continue;
		}
		flag = 1;
		dev_data[i].id = i;
		dev_data[i].valid = 1;
		data->id = i;
		data->valid = 1;
		file->private_data = data;
		printk(KERN_INFO "I2C driver open %d\n",data->id);
	}
	
	if(flag == 0)
	{
		printk(KERN_INFO "I2C driver cannot open more then 2 devices\n");
		return -EBUSY;
	}
	
	printk(KERN_INFO "I2C driver open\n");
	return 0;
}
*/
static int i2c_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset)
{
	return 0;
}

static int i2c_write(struct file *file, const char __user *user_buffer, size_t size, loff_t * offset)
{
	struct driver_data *data = (struct driver_data *)file->private_data;
	uint8_t *buf;
	uint8_t tmp, ack, i;
	
	printk(KERN_INFO "size %d",size);
	buf = (uint8_t *)kmalloc(size+1, GFP_KERNEL);
	if(buf == NULL)
	{
		printk(KERN_INFO "Kmalloc failed...!!!\n");
		return -ENOMEM;
	}
		
	if(copy_from_user(buf, user_buffer, size))
	{
		printk(KERN_INFO "copy from user failed\n");
		kfree(buf);
		return -EFAULT;
	}
	printk(KERN_INFO "slave %d data %d\n",dev_data[data->id].slave_addr,buf[0]);
	
	tmp = *(buf++);
	tmp = tmp << 1;
	i2c_start();
	ack = i2c_write_byte(tmp);
	if(ack==1)
	{
		for(i=0; i<(size-1); i++)
		{
			tmp = *(buf++);
			ack = i2c_write_byte(tmp);
			if(ack == 0)
			{
				printk(KERN_INFO "NACK\n");
				return -1;
			}
		}
	}
	i2c_stop();
	
	kfree(buf);
	return size;
}

static long i2c_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
	struct driver_data *data = (struct driver_data *)file->private_data;
	printk(KERN_INFO "ioctl id %d\n",data->id);
	switch(cmd)
	{
			case I2C_SLAVE: dev_data[data->id].slave_addr = (uint8_t)arg;
							break;
			default: return -ENOTTY;
	}

	return 0;
}

static int i2c_release(struct inode *inode, struct file *file)
{
	struct driver_data *data = file->private_data;
	
	dev_data[data->id].valid = 0;
	return 0;
}

static int i2c_driver_init(void)
{
	int rc;
	struct device *dev_ret;
	
	printk(KERN_INFO "I2C driver entry\n");
	
	rc = register_chrdev(0, "i2c_driver", &f_op);
	if(rc < 0)
	{
		printk(KERN_INFO "Dynamic allocation of Major number failed.\n");
		return rc;
	}
	major = rc; 
	printk(KERN_INFO "Major number is %d", major); 
		
	c = class_create(THIS_MODULE, "i2c_driver");
    if(IS_ERR(c))
    {
		printk(KERN_INFO "Class create failed\n"); 
		class_destroy(c);
		return PTR_ERR(c);
	}
	
	dev_ret = device_create(c, NULL, MKDEV(major,0), NULL, "i2c_driver");
	if(IS_ERR(dev_ret))
    {
		printk(KERN_INFO "Device create failed\n"); 
		class_destroy(c);
		device_destroy(c, MKDEV(major, 0));
		return PTR_ERR(dev_ret);
	}
	
	/*gpio_request(SDA, "sysfs");
	gpio_direction_output(SDA, 1);
	gpio_set_value(SDA, 1);
	gpio_export(SDA, true);
	
	gpio_request(SCL, "sysfs");
	gpio_direction_output(SCL, 1);
	gpio_set_value(SCL, 1);
	gpio_export(SCL, false);
	*/
	return 0;
}

static void i2c_driver_exit(void)
{
	printk(KERN_INFO "I2C driver exit\n");
	
	device_destroy(c, MKDEV(major, 0));
    class_destroy(c);
      
    unregister_chrdev_region(MKDEV(major, 0), MAX_MINOR_NUM);
    /*
    gpio_unexport(SDA);
    gpio_unexport(SCL);*/
}

module_init(i2c_driver_init);
module_exit(i2c_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vatsal Sheth & Sarthak Jain");
MODULE_DESCRIPTION("Basic I2C Driver...!!!");
