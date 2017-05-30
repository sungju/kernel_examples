#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/sched.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
static void my_sysrq_task_list(int key, struct pt_regs *ptregs,
                              struct tty_struct *ttystruct)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static void my_sysrq_task_list(int key, struct tty_struct *ttystruct)
#else
static void my_sysrq_task_list(int key)
#endif
{
  struct task_struct *p;

  printk("*** Start to dump task list ***\n");

  for_each_process(p) {
    printk("%s(%d) is %s Runnable\n",
          p->comm, p->pid,
          p->state == TASK_RUNNING ? "" : "Not");
  }
  printk("*** End of task dumping ***\n");
}

static struct sysrq_key_op my_sysrq_op = {
  .handler = my_sysrq_task_list,
  .help_msg = "dump-All-tasks",
  .action_msg = "Show All Task List",
};

static int __init my_init(void)
{
  int ret = register_sysrq_key('a', &my_sysrq_op);

  printk("SysRq function is %sregistered\n", ret ? "" : "Not ");
  return ret;
}

static void __exit my_exit(void)
{
  unregister_sysrq_key('a', &my_sysrq_op);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
