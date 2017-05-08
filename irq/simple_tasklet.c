#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");

static void tasklet_func(unsigned long data)
{
  printk("Tasklet called.\n");
}

DECLARE_TASKLET(tl_descr, tasklet_func, 0L);

static int __init drv_init(void)
{
  printk("drv_init called\n");
  tasklet_schedule(&tl_descr);
  return 0;
}

static void __exit drv_exit(void)
{
  printk("drv_exit called\n");
  tasklet_kill(&tl_descr);
}

module_init(drv_init);
module_exit(drv_exit);
