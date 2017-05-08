/* timer_drv.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

#define MYDEV_NAME  "mycdrv"
int mycdrv_ma_no;
struct timer_list my_timer;

void my_timer_function(unsigned long ptr)
{
  printk("my_timer_function(), jiffies=%ld\n", jiffies);
  printk("my data = %d, my pid=%d\n", (int)ptr, (int)current->pid);
}

ssize_t my_write(struct file *file, const char *buf, size_t lbuf, loff_t * ppos)
{
  static int len = 100;

  printk("my_write(),current->pid=%d\n", (int)current->pid);
  init_timer(&my_timer);
  my_timer.function = my_timer_function;
  my_timer.expires = jiffies + HZ;
  my_timer.data = len;
  printk("Adding timer at jiffies = %ld\n", jiffies);
  add_timer(&my_timer);
  len += 10;
  return lbuf;
}

struct file_operations fops = {
  .owner = THIS_MODULE,
  .write = my_write,
};

int my_init(void)
{
  mycdrv_ma_no = register_chrdev(0, MYDEV_NAME, &fops);
  return 0;
}

void my_exit(void)
{
  unregister_chrdev(mycdrv_ma_no, MYDEV_NAME);
}

module_init(my_init);
module_exit(my_exit);
