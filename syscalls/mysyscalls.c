/* mysyscall.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/unistd.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>

unsigned long syscall_addr;

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


void (*set_pages_rw)(struct page *page, int numpages);

int __init my_init(void)
{
  int i;
  unsigned long set_pages_rw_addr;
  struct page *sys_call_table_page;

  write_cr0(read_cr0() & 0xfffeffff);

  syscall_addr = kallsyms_lookup_name("sys_call_table");
  sys_call_table = (void *)syscall_addr;

  /* Change the sys_call_tabel to be readable */
  set_pages_rw_addr = kallsyms_lookup_name("set_pages_rw");
  set_pages_rw = (void (*)(struct page*, int))set_pages_rw_addr;
  sys_call_table_page = virt_to_page(sys_call_table);
  set_pages_rw(sys_call_table_page, 1);

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
  if (orig_open != NULL)
    sys_call_table[__NR_open] = (syscall_handler_t *)orig_open;
  write_cr0(read_cr0() | (~0xfffeffff));
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Kwon");
MODULE_DESCRIPTION("System call hooking example");
