#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

static int my_dev_id[NR_IRQS];

irqreturn_t my_handler(int irq, void *data)
{
  return IRQ_HANDLED;
}

static int __init my_init(void) {
  int irq;
  int ret;
  for (irq = 0; irq < NR_IRQS; irq++) {
    ret = request_irq(irq, my_handler, IRQF_SHARED, "myirq", &(my_dev_id[irq]));
    if (ret < 0) {
      my_dev_id[irq] = -1;
    } else {
      my_dev_id[irq] = irq;
      printk("myirq is regstered on %d\n", irq);
    }
  }
  return 0;
}

static void __exit my_exit(void) {
  int irq;
  for (irq = 0; irq < NR_IRQS; irq++) {
    if (my_dev_id[irq] >= 0) {
      free_irq(irq, &(my_dev_id[irq]));
    }
  }
}
module_init(my_init);
module_exit(my_exit);
