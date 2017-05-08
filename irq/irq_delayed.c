#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

int irq, my_dev_id, irq_counter = 0;
module_param(irq, int, 0);

int delay = 0;
module_param(delay, int, 0);

atomic_t counter_bh, counter_th, counter_wq;

struct my_dat {
  unsigned long jiffies;
};
struct my_dat my_data;

void t_func(unsigned long t_arg) {
  struct my_dat *data = (struct my_dat *)t_arg;
  atomic_inc(&counter_bh);
  printk("In tasklet: counter_th = %d, counter_bh = %d, jiffies = %ld,%ld\n",
      atomic_read(&counter_th), atomic_read(&counter_bh),
      data->jiffies, jiffies);
}

DECLARE_TASKLET(t_name, t_func, (unsigned long)&my_data);

struct workqueue_struct *my_workqueue;

void w_func(struct work_struct *work)
{
  atomic_inc(&counter_wq);
  printk("In workqueue: counter_th = %d, counter_wq = %d, jiffies = %ld,%ld\n",
      atomic_read(&counter_th), atomic_read(&counter_wq),
      my_data.jiffies, jiffies);
}
DECLARE_WORK(w_name, w_func);

irqreturn_t my_interrupt(int irq, void *dev_id) {
  struct my_dat *data = (struct my_dat *)dev_id;
  atomic_inc(&counter_th);
  data->jiffies = jiffies;

  tasklet_schedule(&t_name);
  queue_work(my_workqueue, &w_name);

  mdelay(delay);
  return IRQ_NONE;
}

int __init my_init(void) {
  int ret;
  my_workqueue = create_workqueue("my_work");

  atomic_set(&counter_th, 0);
  atomic_set(&counter_bh, 0);
  atomic_set(&counter_wq, 0);
  ret = request_irq(irq, my_interrupt, IRQF_SHARED, "my_int", &my_data);
  printk("Successfully loaded\n");
  return 0;
}

void __exit my_exit(void) {
  free_irq(irq, &my_data);
  printk("counter_th = %d, counter_wq = %d, counter_bh = %d\n",
      atomic_read(&counter_th), atomic_read(&counter_wq), atomic_read(&counter_bh));
  destroy_workqueue(my_workqueue);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
