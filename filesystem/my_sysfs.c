#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>

static struct kobject *example_kobject;
static int foo;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	sscanf(buf, "%du", &foo);
	return count;
}

static struct kobj_attribute foo_attribute = __ATTR(foo, 0660, 
                                                    foo_show, foo_store);

static int __init my_init(void)
{
	int error = 0;

	pr_debug("Module initialized successfully \n");

  /* You can choose any parents you want to use */
  /* kernel_kobj
   * dev_kobj
   * firmware_kobj
   * fs_kobj
   * mm_kobj
   * hypervisor_kobj
   * acpi_kobj
   * ...
   */
	example_kobject = kobject_create_and_add("example_sysfs",
						 kernel_kobj);
	if (!example_kobject)
		return -ENOMEM;

	error = sysfs_create_file(example_kobject, &foo_attribute.attr);
	if (error) {
		pr_debug
		    ("failed to create the foo file in /sys/kernel/example_sysfs \n");
	}

	return error;
}

static void __exit my_exit(void)
{
	pr_debug("Module un initialized successfully \n");
	kobject_put(example_kobject);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
