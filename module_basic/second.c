/* second.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

extern void first_func(void);

MODULE_LICENSE("GPL");

int __init my_init(void) {
    printk("Hello, I'm second\n");
      first_func();
        return 0;
}
void __exit my_exit(void) {
    printk("Goodbye, I'm second\n");
}

module_init(my_init);
module_exit(my_exit);
