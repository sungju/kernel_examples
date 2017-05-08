#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define NODE	"myproc"

int param = 100;
struct proc_dir_entry *my_proc;

ssize_t my_proc_read(struct file *file, char __user * buf,
		     size_t len, loff_t * offset)
{
	char mydata[20];

	sprintf(mydata, "%d\n", param);
	if (*offset > strlen(mydata))
		return 0;
	*offset = strlen(mydata) + 1;
	return strlen(mydata) - copy_to_user(buf, mydata, strlen(mydata));
}

ssize_t my_proc_write(struct file * file, const char __user * buffer,
		      size_t count, loff_t * pos)
{
	char *str;
	str = kmalloc(count, GFP_KERNEL);
	if (copy_from_user(str, buffer, count)) {
		kfree(str);
		return -EFAULT;
	}
	sscanf(str, "%d", &param);
	printk("param has been set to %d\n", param);
	kfree(str);
	return count;
}

struct file_operations myfops = {
	.read = my_proc_read,
	.write = my_proc_write,
};

int __init my_init(void)
{
	my_proc = proc_create(NODE, 0666, NULL, &myfops);
	if (!my_proc) {
		printk("I failed to make %s\n", NODE);
		return -1;
	}
	printk("I created %s\n", NODE);
	return 0;
}

void __exit my_exit(void)
{
	if (my_proc) {
		remove_proc_entry(NODE, NULL);
		printk("Removed %s\n", NODE);
	}
}

module_init(my_init);
module_exit(my_exit);
