/* sema_mod1.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/semaphore.h>

struct semaphore my_sem;
EXPORT_SYMBOL(my_sem);

int my_open(struct inode *inode, struct file *file)
{
	if (down_trylock(&my_sem)) {
		printk("down_trylock error. my_sem.count : %d\n", my_sem.count);
		up(&my_sem);
		return 0;
	}
	printk("open is called. my_sem.count : %d\n", my_sem.count);
	up(&my_sem);
	return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
};

int major_no = 0;
/*-----------------------*/
/* DECLARE_MUTEX(my_sem); */

int my_init(void)
{
	sema_init(&my_sem, 1);
	printk("Init semaphore unlocked, count=%d\n", my_sem.count);
	if (down_interruptible(&my_sem))
		return -1;
	printk("After down, count=%d\n", my_sem.count);

	major_no = register_chrdev(0, "MYDEV", &fops);

	return 0;
}

void my_exit(void)
{
	unregister_chrdev(major_no, "MYDEV");

	up(&my_sem);
	printk("Exiting with semaphore having count = %d\n", my_sem.count);
}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL");
