#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

#define BCM2835_NUM_GPIOS 54
int gpio_no = 18;
module_param(gpio_no, int, 0);

static int __init my_init(void) {
  gpio_no = (ARCH_NR_GPIOS - BCM2835_NUM_GPIOS) + gpio_no;
	if (!gpio_is_valid(gpio_no)) {
		printk(KERN_INFO "This is invalid GPIO\n");
		return -ENODEV;
	}
	gpio_request(gpio_no, "LED_RED");
	gpio_direction_output(gpio_no, 1);
	gpio_export(gpio_no, false);

	gpio_set_value(gpio_no, 1);

	return 0;
}

static void __exit my_exit(void) {
	gpio_set_value(gpio_no, 0);
	gpio_unexport(gpio_no);
	gpio_free(gpio_no);
}

module_init(my_init);
module_exit(my_exit);
