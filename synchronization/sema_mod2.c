/* sema_mod2.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/errno.h>

extern struct semaphore my_sem;

int my_init(void)
{
	printk("Module 2 semaphore count=%d\n", my_sem.count);
	if (down_interruptible(&my_sem)) {
//        if (down_trylock(&my_sem)) {
		printk("Not loading module; failed\n");
		return -EBUSY;
	}
	printk("Module 2 semaphore down() count = %d\n", my_sem.count);
	return 0;
}

void my_exit(void)
{
	up(&my_sem);
	printk("Module2 semaphore end count=%d\n", my_sem.count);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
