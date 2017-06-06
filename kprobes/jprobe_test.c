#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

static long my_fork(unsigned long clone_flags, unsigned long stack_start,
                    struct pt_regs *regs, unsigned long stack_size,
                    int __user *parent_tidptr, int __user *child_tidptr)
{
  printk("jprobe my_fork: clone_flags = 0x%lx, stack_size = 0x%lx\n",
      clone_flags, stack_size);

  jprobe_return();

  printk("After actual do_fork()? No, not going to be called\n");
  return 0;
}

static struct jprobe my_jprobe = {
  .entry = my_fork,
  .kp = {
    .symbol_name = "do_fork",
  },
};

static int __init my_init(void)
{
  int ret;

  ret = register_jprobe(&my_jprobe);
  if (ret < 0) {
    printk("register_jprobe failed with %d\n", ret);
    return -1;
  }

  printk("register_jprobe installed\n");
  return 0;
}

static void __exit my_exit(void)
{
  unregister_jprobe(&my_jprobe);
  printk("jprobe unregistered\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
