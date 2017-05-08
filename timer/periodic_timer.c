/* periodic_timer.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

struct timer_list timer;
struct kt_data {
  unsigned long period;
  unsigned long start_time;
} data;

void ktfun(unsigned long var)
{
  struct kt_data *tdata = (struct kt_data *)var;
  printk("ktimer:period=%ld elapsed =%ld\n",
         tdata->period, jiffies - tdata->start_time);
  mod_timer(&timer, tdata->period + jiffies);
}

int init_module(void)
{
  data.period = 2 * HZ;
  init_timer(&timer);
  timer.function = ktfun;
  timer.data = (unsigned long)&data;
  timer.expires = jiffies + data.period;
  data.start_time = jiffies;
  add_timer(&timer);
  return 0;
}

void cleanup_module(void)
{
  printk("Delete timer,rc=%d\n", del_timer_sync(&timer));
}
