/* first.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int __init my_init(void) {
  printk("Hello, I'm first\n");

  return 0;
}
void __exit my_exit(void) {
  printk("Bye, I'm first\n");
}

void first_func(void) {
  printk("You call me, I'm first_func\n");
}

EXPORT_SYMBOL(first_func);

module_init(my_init);
module_exit(my_exit);
