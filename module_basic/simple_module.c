#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Module Author");
MODULE_DESCRIPTION("Module Description");

static int __init my_init(void) {
  return 0; 
}

static void __exit my_exit(void) {
  return;
}

module_init(my_init);
module_exit(my_exit);
