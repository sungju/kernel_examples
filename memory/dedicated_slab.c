#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

kmem_buckets *my_buckets;
void *data;
int size = 100;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sungju Kwon");
MODULE_DESCRIPTION("Dedicated slab example");

static int __init my_init(void)
{
	my_buckets = kmem_buckets_create("my_bucket_cache",
					 SLAB_NO_MERGE, 0, 0, NULL);
	if (!my_buckets) {
		pr_err("Failed to create dedicated bucket cache\n");
		return -ENOMEM;
	}

	data = kmem_buckets_alloc(my_buckets, size, GFP_KERNEL);
	if (!data) {
		pr_err("Allocation from dedicated bucket failed\n");
		return -ENOMEM;
	}
	return 0;
}

static void __exit my_exit(void)
{
	kfree(data);
	return;
}

module_init(my_init);
module_exit(my_exit);

