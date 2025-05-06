#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ctype.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sungju Kwon");
MODULE_DESCRIPTION("kmalloc randomness example");

void hex_dump(const void *buf, size_t len)
{
    const unsigned char *data = buf;
    size_t i, j;
    char line[128];

    for (i = 0; i < len; i += 16) {
        int pos = 0;

        // Offset
        pos += snprintf(line + pos, sizeof(line) - pos, "%08zx  ", i);

        // Hex bytes
        for (j = 0; j < 16; j++) {
            if (i + j < len)
                pos += snprintf(line + pos, sizeof(line) - pos, "%02x ", data[i + j]);
            else
                pos += snprintf(line + pos, sizeof(line) - pos, "   ");
        }

        // ASCII representation
        pos += snprintf(line + pos, sizeof(line) - pos, " |");
        for (j = 0; j < 16 && (i + j) < len; j++) {
            unsigned char c = data[i + j];
            pos += snprintf(line + pos, sizeof(line) - pos, "%c", isprint(c) ? c : '.');
        }
        snprintf(line + pos, sizeof(line) - pos, "|");

        pr_info("%s\n", line);
    }
}

static int __init my_init(void)
{
	int size = 1024; // Make it use kmalloc-1k which allocates 4 entries per slab
        char *data[4];
	int i;

	printk("Allocating and setting the value\n");
	for (i = 0; i < 4; i++) {
		data[i] = (char *)kmalloc(size, GFP_KERNEL);
		if (data[i] != NULL) {
			memset(data[i], i, size);
			hex_dump(data[i], 32);
		}
	}
	printk("Freeing the slabs\n");
	for (i = 0; i < 4; i++) {
		if (data[i] != NULL) 
			kfree(data[i]);
	}
	printk("Allocating SLABs and check the previous value\n");
	for (i = 0; i < 4; i++) {
		data[i] = (char *)kmalloc(size, GFP_KERNEL);
		if (data[i] != NULL)
			hex_dump(data[i], 32);
	}
	printk("Freeing the slabs\n");
	for (i = 0; i < 4; i++) {
		if (data[i] != NULL) 
			kfree(data[i]);
	}
	printk("Done\n");

	return 0;
}

static void __exit my_exit(void)
{
	return;
}

module_init(my_init);
module_exit(my_exit);

