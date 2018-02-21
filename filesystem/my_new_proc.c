/* my_new_proc.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>

#define NODE_DIR	"my_proc_dir"
#define NODE_1		"param1"
#define NODE_2		"param2"

int param_1 = 100, param_2 = 200;
struct proc_dir_entry *my_proc_dir, *my_proc_1, *my_proc_2;

ssize_t my_proc_read(struct file *file, char __user * buf,
		     size_t len, loff_t * offset)
{
	char mydata[20];

	if (PDE_DATA(file_inode(file)) == (void *)1)
		sprintf(mydata, "%d\n", param_1);
	if (PDE_DATA(file_inode(file)) == (void *)2)
		sprintf(mydata, "%d\n", param_2);

	if (*offset > strlen(mydata))
		return 0;
	*offset = strlen(mydata) + 1;
	return strlen(mydata) - copy_to_user(buf, mydata, strlen(mydata));
}

ssize_t my_proc_write(struct file * file, const char __user * buffer,
		      size_t count, loff_t * pos)
{
	char *str = kmalloc(count, GFP_KERNEL);
	if (copy_from_user(str, buffer, count)) {
		kfree(str);
		return -EFAULT;
	}
	if (PDE_DATA(file_inode(file)) == (void *)1) {
		sscanf(str, "%d", &param_1);
		printk("param_1 has been set to %d\n", param_1);
		kfree(str);
		return count;
	}
	if (PDE_DATA(file_inode(file)) == (void *)2) {
		sscanf(str, "%d", &param_2);
		printk("PARAM2 is set to %d\n", param_2);
		kfree(str);
		return count;
	}
	kfree(str);
	return -EINVAL;
}

struct file_operations myfops = {
	.read = my_proc_read,
	.write = my_proc_write,
};

int __init my_init(void)
{
	my_proc_dir = proc_mkdir(NODE_DIR, NULL);

	my_proc_1 =
	    proc_create_data(NODE_1, 0666, my_proc_dir, &myfops, (void *)1);

	my_proc_2 =
	    proc_create_data(NODE_2, 0666, my_proc_dir, &myfops, (void *)2);

	return 0;
}

void __exit my_exit(void)
{
	remove_proc_entry(NODE_1, my_proc_dir);
	remove_proc_entry(NODE_2, my_proc_dir);
	remove_proc_entry(NODE_DIR, NULL);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
