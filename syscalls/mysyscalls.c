/* mysyscall.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/unistd.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/syscalls.h> 

unsigned long syscall_addr;
module_param(syscall_addr, ulong, 0);

typedef long syscall_handler_t(void);
syscall_handler_t **sys_call_table;

asmlinkage long (*orig_open)(const char __user *filename, int flags, umode_t mode);

//SYSCALL_DEFINE3(myopen, const char __user *, filename, int, flags, umode_t, mode)
asmlinkage long myopen(const char __user *filename, int flags, umode_t mode)
{
  char tmpname[256];
  int length;

  length = 256 - copy_from_user(tmpname, filename, 255);
  printk("opening %s by %s\n", tmpname, current->comm);
  if (!orig_open)
    return -1;

  return orig_open(filename, flags, mode);
}

int __init my_init(void)
{
  int i;
  sys_call_table = (void *)syscall_addr;

  for (i = 0; i < 20; i++) {
    printk("syscall[%i] = %p\n", i, sys_call_table[i]);
  }
  printk("open is at %d = %p\n", __NR_open, sys_call_table[__NR_open]);
  orig_open = (void *)sys_call_table[__NR_open];
  sys_call_table[__NR_open] = (syscall_handler_t *)myopen;

  return 0;
}

void __exit my_exit(void)
{
  sys_call_table[__NR_open] = (syscall_handler_t *)orig_open;
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Kwon");
MODULE_DESCRIPTION("System call hooking example");
