#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/version.h>

static unsigned long address = 0x0;
static char *name = 0;
module_param(address, ulong, S_IRUGO);
module_param(name, charp, S_IRUGO);

static struct kprobe kp;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	printk("pre_handler called before p->addr=0x%p\n", p->addr);
	dump_stack();
	return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs,
			 unsigned long flags)
{
	printk("post_handler called after p->addr=0x%p\n", p->addr);
	dump_stack();
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	printk("fault_handler called during calling p->addr=0x%p\n", p->addr);
	dump_stack();
	return 0;
}

static int __init my_init(void)
{
	if (address == 0 && (name == 0 || strlen(name) == 0)) {
		printk
		    ("Target function is not specified. Please use address or name to monitoring it\n");
		return -1;
	}
	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,12)
	if (name != NULL)
		address = kallsyms_lookup_name(name);
#endif

	if (!address) {
		printk("Can't find the target address for %s\n", name);
		return -1;
	}

	kp.addr = (kprobe_opcode_t *) address;

	if (register_kprobe(&kp)) {
		printk("Can't register kprobe on %s\n", name);
		return -1;
	}
	printk("Hello, kprobe is registered\n");

	return 0;
}

static void __exit my_exit(void)
{
	unregister_kprobe(&kp);
	printk("Bye bye\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
