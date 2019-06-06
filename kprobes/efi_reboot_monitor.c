#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/efi.h>
#include <asm/efi.h>
#include <linux/acpi.h>

void do_final_job(char *msg)
{
	dump_stack();
	panic(msg);
}

void my_native_machine_power_off(void)
{
	do_final_job("HR: native_machine_power_off() called\n");
}

static struct jprobe my_jprobe_native_machine_power_off = {
	.entry = my_native_machine_power_off,
	.kp = {
	       .symbol_name = "native_machine_power_off",
	       },
};

void my_native_machine_shutdown(void)
{
	do_final_job("HR: native_machine_shutdown() called\n");
}

static struct jprobe my_jprobe_native_machine_shutdown = {
	.entry = my_native_machine_shutdown,
	.kp = {
	       .symbol_name = "native_machine_shutdown",
	       },
};

static void my_native_machine_emergency_restart(void)
{
	do_final_job("HR: native_machine_emergency_restart() called\n");
}

static struct jprobe my_jprobe_native_machine_emergency_restart = {
	.entry = my_native_machine_emergency_restart,
	.kp = {
	       .symbol_name = "native_machine_emergency_restart",
	       },
};

static void my_native_machine_restart(char *__unused)
{
	do_final_job("HR: native_machine_restart() called\n");
}

static struct jprobe my_jprobe_native_machine_restart = {
	.entry = my_native_machine_restart,
	.kp = {
	       .symbol_name = "native_machine_restart",
	       },
};

static void my_native_machine_halt(void)
{
	do_final_job("HR: native_machine_halt() called\n");
}

static struct jprobe my_jprobe_native_machine_halt = {
	.entry = my_native_machine_halt,
	.kp = {
	       .symbol_name = "native_machine_halt",
	       },
};

void my_native_machine_crash_shutdown(struct pt_regs *regs)
{
	do_final_job("HR: native_machine_crash_shutdown() called\n");
}

static struct jprobe my_jprobe_native_machine_crash_shutdown = {
	.entry = my_native_machine_crash_shutdown,
	.kp = {
	       .symbol_name = "native_machine_crash_shutdown",
	       },
};

void my_efi_call(void *args)
{
	printk(KERN_ERR "HR: ec();\n");
	jprobe_return();
}

static struct jprobe my_jprobe_efi_call = {
	.entry = my_efi_call,
	.kp = {
	       .symbol_name = "efi_call",
	       },
};


char efi_name[50];
void set_efi_name(efi_char16_t * msg)
{
	u16 uchar;
	int cnt = 0;
	memset(efi_name, 0, 50);
	while ((uchar = *msg++) != 0) {
		efi_name[cnt++] = (char)uchar;
	}
}

char vendor_guid[37];

void set_guid(efi_guid_t * vendor)
{
	int i = 0, cnt = 0;
	memset(vendor_guid, 0, 37);
	for (i = 3; i >= 0; i--) {
		sprintf(&vendor_guid[cnt], "%02x", vendor->b[i]);
		cnt = cnt + 2;
	}
	vendor_guid[cnt] = '-'; cnt++;
	for (i = 5; i >= 4; i--) {
		sprintf(&vendor_guid[cnt], "%02x", vendor->b[i]);
		cnt = cnt + 2;
	}
	vendor_guid[cnt] = '-'; cnt++;
	for (i = 7; i >= 6; i--) {
		sprintf(&vendor_guid[cnt], "%02x", vendor->b[i]);
		cnt = cnt + 2;
	}
	vendor_guid[cnt] = '-'; cnt++;
	for (i = 8; i < 10; i++) {
		sprintf(&vendor_guid[cnt], "%02x", vendor->b[i]);
		cnt = cnt + 2;
	}
	vendor_guid[cnt] = '-'; cnt++;
	for (i = 10; i < 16; i++) {
		sprintf(&vendor_guid[cnt], "%02x", vendor->b[i]);
		cnt = cnt + 2;
	}
}

efi_status_t my_virt_efi_get_variable(efi_char16_t * name,
				      efi_guid_t * vendor,
				      u32 * attr,
				      unsigned long *data_size, void *data)
{
	set_efi_name(name);
	set_guid(vendor);
	printk(KERN_ERR "HR: vegv(), name : %s-%s\n", efi_name, vendor_guid);
	jprobe_return();
	return 0;
}

static struct jprobe my_jprobe_virt_efi_get_variable = {
	.entry = my_virt_efi_get_variable,
	.kp = {
	       .symbol_name = "virt_efi_get_variable",
	       },
};

efi_status_t my_virt_efi_set_variable(efi_char16_t * name,
                                      efi_guid_t * vendor,
                                      u32 * attr,
                                      unsigned long *data_size, void *data)
{
	set_efi_name(name);
	set_guid(vendor);
	printk(KERN_ERR "HR: vesv(), name : %s-%s\n", efi_name, vendor_guid);
	jprobe_return();
	return 0;
}

static struct jprobe my_jprobe_virt_efi_set_variable = {
        .entry = my_virt_efi_set_variable,
        .kp = {
               .symbol_name = "virt_efi_set_variable",
               },
};


void virt_efi_get_variable_post_handler(struct kprobe *kp, struct pt_regs *regs,
					unsigned long flags)
{
	printk(KERN_ERR "HR: vegv()_post\n");
}

static struct kprobe virt_efi_get_variable_kprobe = {
	.symbol_name = "virt_efi_get_variable",
	.post_handler = virt_efi_get_variable_post_handler
};

void virt_efi_set_variable_post_handler(struct kprobe *kp, struct pt_regs *regs,
                                        unsigned long flags)
{
	printk(KERN_ERR "HR: vesv()_post\n");
}

static struct kprobe virt_efi_set_variable_kprobe = {
        .symbol_name = "virt_efi_set_variable",
        .post_handler = virt_efi_set_variable_post_handler
};

int efivar_entry_size_pre_handler(struct kprobe *kp, struct pt_regs *regs)
{
	printk(KERN_ERR "HR: ees()_pre\n");
	return 0;
}

void efivar_entry_size_post_handler(struct kprobe *kp, struct pt_regs *regs,
				    unsigned long flags)
{
	printk("HR: ees()_post\n");
}

static struct kprobe efivar_entry_size_kprobe = {
	.symbol_name = "efivar_entry_size",
	.pre_handler = efivar_entry_size_pre_handler,
	.post_handler = efivar_entry_size_post_handler,
};

#define NUMKP 3
static struct kprobe *kps[NUMKP] = {
	&virt_efi_get_variable_kprobe,
	&virt_efi_set_variable_kprobe,
	&efivar_entry_size_kprobe
};

unsigned int myjp_acpi_os_write_port(acpi_io_address port, u32 value, u32 width)
{
	if (port == 0xcf9) {
		printk(KERN_ERR
		       "acpi_os_write_port: port=0x%llx, value=%u, width=%u\n",
		       port, value, width);
		dump_stack();
	}
	jprobe_return();
	return 0;
}

int myjp_ioperm(unsigned long from, unsigned long num, int turn_on)
{
	if ((0xcf9 >= from && 0xcf9 <= (from + num)) ||
	    (0x64 >= from && 0x64 <= (from + num))) {
		printk(KERN_ERR "ioperm: from=0x%lx, num=%lu, turn_on=%d\n",
		       from, num, turn_on);
		dump_stack();
	}
	jprobe_return();
	return 0;
}

int myjp_iopl(int level)
{
	printk(KERN_ERR "iopl: level=%d\n", level);
	dump_stack();
	jprobe_return();
	return 0;
}

static struct jprobe jp1 = {.entry = myjp_acpi_os_write_port,.kp =
	    {.symbol_name = "acpi_os_write_port",},
};
static struct jprobe jp2 = {.entry = myjp_ioperm,.kp = {.symbol_name =
							"sys_ioperm",},
};
static struct jprobe jp3 = {.entry = myjp_iopl,.kp = {.symbol_name =
						      "sys_iopl",},
};

#define NUMJP 12
static struct jprobe *jps[NUMJP] = { &jp1, &jp2, &jp3,
	&my_jprobe_native_machine_power_off,
	&my_jprobe_native_machine_shutdown,
	&my_jprobe_native_machine_emergency_restart,
	&my_jprobe_native_machine_restart,
	&my_jprobe_native_machine_halt,
	&my_jprobe_native_machine_crash_shutdown,
	&my_jprobe_efi_call,
	&my_jprobe_virt_efi_get_variable,
	&my_jprobe_virt_efi_set_variable
};

int register_jprobe_list(void)
{
	int i, j, ret;

	for (i = 0; i < NUMJP; i++) {
		ret = register_jprobe(jps[i]);
		if (ret != 0) {
			for (j = i - 1; j >= 0; j--)
				unregister_jprobe(jps[j]);

			printk(KERN_ERR
			       "failed to register jprobe(%s) from %s.ko (ret %d)\n",
			       jps[i]->kp.symbol_name, THIS_MODULE->name, ret);
			return 1;
		} else
			printk(KERN_ERR
			       "%d:Installed jprobe %s at %p (ret %d)\n", i,
			       jps[i]->kp.symbol_name, jps[i]->kp.addr, ret);
	}
	return 0;
}

void unregister_jprobe_list(void)
{
	int i;

	for (i = 0; i < NUMJP; i++) {
		if (jps[i]->kp.addr != 0) {
			unregister_jprobe(jps[i]);
			printk(KERN_ERR
			       "%d: unregister jprobe(%s) from %s.ko\n", i,
			       THIS_MODULE->name, jps[i]->kp.symbol_name);
		}
	}
}

int register_kprobe_list(void)
{
	int i, j, ret;

	for (i = 0; i < NUMKP; i++) {
		ret = register_kprobe(kps[i]);
		if (ret != 0) {
			for (j = i - 1; j >= 0; j--)
				unregister_kprobe(kps[j]);

			printk(KERN_ERR
			       "failed to register kprobe(%s) from %s.ko (ret %d)\n",
			       kps[i]->symbol_name, THIS_MODULE->name, ret);
			return 1;
		} else
			printk(KERN_ERR
			       "%d:Installed kprobe %s at %p (ret %d)\n", i,
			       kps[i]->symbol_name, kps[i]->addr, ret);
	}
	return 0;
}

void unregister_kprobe_list(void)
{
	int i;

	for (i = 0; i < NUMKP; i++) {
		if (kps[i]->addr != 0) {
			unregister_kprobe(kps[i]);
			printk(KERN_ERR
			       "%d: unregister kprobe(%s) from %s.ko\n", i,
			       THIS_MODULE->name, kps[i]->symbol_name);
		}
	}
}

static int __init my_init(void)
{
	int ret;

	ret = register_jprobe_list();
	if (ret > 0)
		return ret;

	ret = register_kprobe_list();
	if (ret > 0) {
		unregister_jprobe_list();
		return ret;
	}

	printk(KERN_ERR "kprobe/jprobes are installed\n");
	return 0;
}

static void __exit my_exit(void)
{
	unregister_kprobe_list();
	unregister_jprobe_list();
	printk(KERN_ERR "jprobe unregistered\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
