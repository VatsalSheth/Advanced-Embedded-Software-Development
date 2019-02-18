#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>

static int time_val, count=0;
static char *name;
struct timer_list mod_time;

void mod_time_handler(unsigned long dummy)
{
	count++;
	printk(KERN_ALERT "Name is %s and count = %d",name, count);
	
	mod_time.expires = jiffies + msecs_to_jiffies(time_val);
	mod_time.data = 0;
	mod_time.function = mod_time_handler;
	add_timer(&mod_time);
}

static int timer_init(void)
{
	printk(KERN_ALERT "Timer Module entry\n");
	if(time_val != 0)
	{
		time_val = time_val * 1000;
	}
	else
	{
		time_val = 500;
	}
	init_timer(&mod_time);
	
	mod_time.expires = jiffies + msecs_to_jiffies(time_val);
	mod_time.data = 0;
	mod_time.function = mod_time_handler;
	add_timer(&mod_time);
	
	return 0;
}

static void timer_exit(void)
{
	printk(KERN_ALERT "Timer Module exit\n");
	del_timer_sync(&mod_time);
}

module_init(timer_init);
module_exit(timer_exit);
module_param_named(secs, time_val, int, 0);
module_param(name, charp, 0);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vatsal Sheth");
MODULE_DESCRIPTION("Timer module for test...!!!");
