#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/time.h>

struct ecology
{
	int count;
	char *name;
	struct list_head list;
};

struct ecology *animal_new;
struct ecology *animal_filter;
struct ecology *tmp;
static LIST_HEAD(animal_head);
static LIST_HEAD(filter_head);

struct timeval start = {0,0};
struct timeval final = {0,0};
struct timeval diff = {0,0};
	
static char *animal[50] = {	"dog", "cat", "bee", "bat", "dolphin",
							"elephant", "frog", "whale", "tiger", "giraffe",
							"squirrel", "ant", "wasp", "bat", "tiger",
							"cat", "whale", "dog", "giraffe", "elephant",
							"bat", "giraffe", "penguin", "whale", "dolphin",
							"dog", "penguin", "squirrel", "frog", "dolphin",
							"penguin", "dolphin", "dolphin", "elephant", "dog",
							"giraffe", "whale", "penguin", "tiger", "frog",
							"bee", "dolphin", "wasp", "frog", "dog",
							"bat", "bee", "penguin", "ant", "penguin"};
int animal_size = 50;
char *filter_name = "empty";
int filter_count = 0;

void time_diff(struct timeval *time2, struct timeval *time1, struct timeval *time_res)
{
  int dt_sec=time2->tv_sec - time1->tv_sec;
  int dt_usec=time2->tv_usec - time1->tv_usec;

  if(dt_sec >= 0)
  {
    if(dt_usec >= 0)
    {
      time_res->tv_sec=dt_sec;
      time_res->tv_usec=dt_usec;
    }
    else
    {
      time_res->tv_sec=dt_sec-1;
      time_res->tv_usec=1000000+dt_usec;
    }
  }
}

static int compar_sort(const void* a, const void* b)
{
	return strcmp (*(const char**)a, *(const char**)b);
}

static int linked_list_init(void)
{
	int i, tmp_count=1, mem_cnt=0, node=0;
	
	do_gettimeofday(&start);
	
	printk(KERN_ALERT "Linked List Module entry\n");
	
	sort(animal, animal_size, sizeof(animal[0]), compar_sort, NULL);
	
	printk(KERN_ALERT "\nAlphabetically sorted Animals:\n");
	
	for(i=0; i<animal_size; i++)
	{
		if((i==(animal_size-1)) || (strcmp(animal[i], animal[i+1])))
		{
			animal_new = (struct ecology *)kmalloc(sizeof(struct ecology),GFP_KERNEL);
			animal_new->count = tmp_count;
			animal_new->name = animal[i];
			list_add_tail(&animal_new->list, &animal_head);
			tmp_count = 1;
			printk(KERN_ALERT "%s\n", animal[i]);
			mem_cnt++;
			node++;
		}
		else
		{
			tmp_count++;
			printk(KERN_ALERT "%s\tDuplicate...!!!\n", animal[i]);
		}
	}
	
	printk(KERN_ALERT "\nAnimals sorted list:\n");
	
	list_for_each_entry(tmp, &animal_head, list)
	{
		printk(KERN_ALERT "name is %s and count = %d\n", tmp->name, tmp->count);
		
		if((tmp->count > filter_count) && ((!strcmp(tmp->name, filter_name)) || (!strcmp(filter_name, "empty"))))
		{
			animal_filter = (struct ecology *)kmalloc(sizeof(struct ecology),GFP_KERNEL);
			animal_filter->count = tmp->count;
			animal_filter->name = tmp->name;
			list_add_tail(&animal_filter->list, &filter_head);
		}
	}
	
	printk(KERN_ALERT "\nList 1 Nodes = %d\n",node);
	printk(KERN_ALERT "List 1 Memory allocated = %d bytes\n",(mem_cnt*sizeof(struct ecology)));
	
	mem_cnt = 0;
	node = 0;
	if(filter_head.next != &filter_head)
	{
		printk(KERN_ALERT "\nAnimals filtered list:\n");
		list_for_each_entry(tmp, &filter_head, list)
		{
			printk(KERN_ALERT "Filter name is %s and count = %d\n", tmp->name, tmp->count);
			mem_cnt++;
			node++;
		}
		printk(KERN_ALERT "\nList 2 Nodes = %d\n",node);
		printk(KERN_ALERT "List 2 Memory allocated = %d bytes\n",(mem_cnt*sizeof(struct ecology)));
	}
	else
	{
		printk(KERN_ALERT "\nNO match for set filter...!!!\n");
	}
	
	do_gettimeofday(&final);
	time_diff(&final, &start, &diff);
	printk(KERN_ALERT "Module entry execution Time = %ld sec, %ld msec (%ld microsec)\n", diff.tv_sec, (diff.tv_usec / 1000), diff.tv_usec);
	
	return 0;
}

static void linked_list_exit(void)
{
	int mem_cnt = 0;
	
	do_gettimeofday(&start);
	
	printk(KERN_ALERT "Linked List Module exit\n");
	list_for_each_entry(tmp, &animal_head, list)
	{
		kfree(tmp);
		mem_cnt++;
	}
	
	printk(KERN_ALERT "\nList 1 Memory freed = %d bytes\n",(mem_cnt*sizeof(struct ecology)));
	mem_cnt = 0;
	
	if(filter_head.next != &filter_head)
	{
		list_for_each_entry(tmp, &filter_head, list)
		{
			kfree(tmp);
			mem_cnt++;
		}
	}
	
	printk(KERN_ALERT "\nList 2 Memory freed = %d bytes\n",(mem_cnt*sizeof(struct ecology)));
	
	do_gettimeofday(&final);
	time_diff(&final, &start, &diff);
	printk(KERN_ALERT "Module exit execution Time = %ld sec, %ld msec (%ld microsec)\n", diff.tv_sec, (diff.tv_usec / 1000), diff.tv_usec);	
}

module_init(linked_list_init);
module_exit(linked_list_exit);
module_param_array(animal, charp, &animal_size, 0);
MODULE_PARM_DESC(animal, "List of animals in zoo...!!!");
module_param(filter_count, int, 0);
MODULE_PARM_DESC(filter_count, "Filter animals with count greater then this\n");
module_param(filter_name, charp, 0);
MODULE_PARM_DESC(filter_name, "Filter animals with this name\n");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vatsal Sheth");
MODULE_DESCRIPTION("Linked List module for test...!!!");
