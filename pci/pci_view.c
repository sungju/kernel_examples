/* pci_view */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/errno.h>
#include <linux/init.h>

int __init my_init(void)
{
	u16 dval;
	char byte;
	int j = 0;
	struct pci_dev *pdev = NULL;

	printk("Loading the pci_device_finder\n");

	while ((pdev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pdev))) {
		printk("\n FOUND PCI DEVICE # j = %d, ", j++);
		pci_read_config_word(pdev, PCI_VENDOR_ID, &dval);
		printk("PCI_VENDOR_ID=%x ", dval);
		pci_read_config_word(pdev, PCI_DEVICE_ID, &dval);
		printk("PCI_DEVICE_ID=%x ", dval);
		pci_read_config_byte(pdev, PCI_INTERRUPT_LINE, &byte);
		printk("irq = %d\n", byte);
	}
	return -1;
}

void __exit my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);
