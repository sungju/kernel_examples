/* hello.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

int irq;
module_param(irq, int, 0644);
int sample;
module_param_named(test, sample, int, 0);

int arr_data[10];
int arr_cnt;
module_param_array(arr_data, int, &arr_cnt, 0);

int my_data __initdata = 5;

int __init my_init(void) {
        printk("irq = %d\n", irq);
        return 0;
}

void __exit my_exit(void) {
        printk("Bye. Bye..%d\n", irq);
}
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sungju");
MODULE_DESCRIPTION("Hello..description");
