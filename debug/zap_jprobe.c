#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

void my_zap_pid_ns_processes(struct pid_namespace *pid_ns)
{
	printk("Hello\n");
  jprobe_return();
}

static struct jprobe my_jprobe = {
  .entry = my_zap_pid_ns_processes,
  .kp = {
//    .symbol_name = "zap_pid_ns_processes",
    .addr = 0xffffffff81172670,
  },
};

static int __init my_init(void)
{
  int ret;

  ret = register_jprobe(&my_jprobe);
  if (ret < 0) {
    printk("register_jprobe failed with %d\n", ret);
    return ret;
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
